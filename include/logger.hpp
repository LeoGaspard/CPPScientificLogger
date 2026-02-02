#pragma once

#if defined(ARMA_INCLUDES)
    #define HAS_ARMADILLO
#endif

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <complex>

enum class LogFmt {Fixed, Scientific, Int};

struct Col {
    LogFmt type;
    int width;
    int prec;
};

class Logger{
    public:
        void printHeader(const std::string& text) {
            std::string top = "╭" + repeat("─", text.length() + 2) + "╮";
            std::string mid = "│ " + text + " │";
            std::string bot = "╰" + repeat("─", text.length() + 2) + "╯";

            std::cout << std::string((WIDTH - text.length()) / 2 , ' ') << top << "\n";
            std::cout << std::string((WIDTH - text.length()) / 2 , ' ') << mid << "\n";
            std::cout << std::string((WIDTH - text.length()) / 2 , ' ') << bot << "\n";
        }

        void printTitleBar(const std::string& title) {
            std::string inner = "┨ " + title + " ┠";
            int side_len = (WIDTH - inner.length() + 4 ) / 2;

            std::cout << " " << repeat(" ", side_len) << "┏" << repeat("━", inner.length() - 6) << "┓" << repeat(" ", side_len) << " " << "\n";
            std::cout << "┌" << repeat("─", side_len) << inner << repeat("─",side_len+1) << "┐" << "\n";
            std::cout << "│" << repeat(" ", side_len) << "┗" << repeat("━", inner.length() - 6) << "┛" << repeat(" ", side_len+1) << "│" << "\n";
        }

        void printSection(const std::string& label) {
            std::cout << "│ ╭" << repeat("─", label.length() + 2) << "╮" << repeat(" ", WIDTH - label.length() - 5) << "│\n";
            std::cout << "│ │ " << label << " │" << repeat(" ",WIDTH - label.length() - 5) << "│\n";
            std::cout << "│ ╰" << repeat("─", label.length() + 2) << "╯" << repeat(" ", WIDTH - label.length() - 5) << "│\n";
        }

        void printClosing() {
            std::cout << "└" << repeat("─", WIDTH) << "┘" << std::endl;
        }

        void printParam(const std::string& key, const std::string& value, bool borders=true) {
            formatAndPrint(key, value, borders);
        }

        void printParam(const std::string& key, bool value, bool borders=true) {
            formatAndPrint(key, value ? "true" : "false", borders);
        }

        void printParam(const std::string& key, const int& value, bool borders=true) {
            std::ostringstream oss; 
            oss << value;
            formatAndPrint(key, oss.str(), borders);
        }

        void printParam(const std::string& key, double value, int decimals=2, bool borders=true) {
            std::ostringstream oss;
            oss << std::showpos << std::fixed << std::setprecision(decimals) << value;
            formatAndPrint(key, oss.str(), borders);
        }

        template <typename T>
        void printParam(const std::string& key, std::complex<T> value, int decimals=2, bool borders=true) {
            std::ostringstream oss;
            oss << std::showpos << std::fixed << std::setprecision(decimals) << value.real() << value.imag() << "i";
            formatAndPrint(key, oss.str(), borders);
        }

#ifdef HAS_ARMADILLO 
        void printParam(const std::string& key, arma::mat M, int decimals=2, bool borders=true) {
            std::string dims = "[" + std::to_string(M.n_rows) + "x" + std::to_string(M.n_cols) + "]";
            formatAndPrint(key, dims, borders);
            for (arma::uword i=0; i<M.n_rows; ++i) {
                std::ostringstream oss;
                if (i==0) oss << "[[";
                else      oss << " [";
                for (arma::uword j=0; j<M.n_cols; ++j) {
                    oss << std::showpos << std::fixed << std::setprecision(decimals) << M(i,j);
                    if (j < M.n_cols -1) oss << ", ";
                }
                if (i == M.n_rows -1) oss << "]]";
                else                  oss << "],";
                formatAndPrint("", oss.str(), borders);
            }
        }

        void printParam(const std::string& key, arma::rowvec V, int decimals=2, bool borders=true) {
            std::ostringstream oss;
            oss << "[";
            for (arma::uword i=0; i<V.n_cols; ++i) {
                oss << std::showpos << std::fixed << std::setprecision(decimals) << V(i);
                if (i < V.n_cols -1) oss << ", ";
            }
            oss << "]";
            formatAndPrint(key, oss.str(), borders);
        }

        void printParam(const std::string& key, arma::colvec V, int decimals=2, bool borders=true) {
            std::ostringstream oss;
            oss << "[";
            for (arma::uword i=0; i<V.n_rows; ++i) {
                oss << std::showpos << std::fixed << std::setprecision(decimals) << V(i);
                if (i < V.n_rows -1) oss << ", ";
            }
            oss << "]";
            formatAndPrint(key, oss.str(), borders);
        }
#endif

        void initTable(const std::vector<Col>& specs) {
            table_specs = specs;
        }

        template <typename... Args>
        void printTableHeader(Args... args) {
            if (sizeof...(args) != table_specs.size()) return;

            std::cout << "┌";
            for (size_t i=0; i<table_specs.size(); ++i) {
                std::cout << repeat("─", table_specs[i].width);
                if (i < table_specs.size() -1) std::cout << "┬";
            }
            std::cout << "┐\n";

            std::cout << "│";
            printHeaderCells(0, args...);

            std::cout << "├";
            for (size_t i=0; i<table_specs.size(); ++i) {
                std::cout << repeat("─", table_specs[i].width);
                if (i < table_specs.size()-1) std::cout << "┼";
            }
            std::cout << "┤\n";
        }

        template <typename... Args> 
        void logRow(Args... args) {
            printCells(0, args...);
        }

        void closeTable(){
            std::cout << "└";
            for (size_t i=0; i<table_specs.size(); ++i) {
                std::cout << repeat("─", table_specs[i].width);
                if (i < table_specs.size() - 1) std::cout << "┴";
            }
            std::cout << "┘\n";
        }

    private:
       static constexpr int WIDTH=80;

       std::vector<Col> table_specs;

       std::string repeat(std::string str, unsigned int n) {
           std::string result;
           result.reserve(str.size()*n);
           for (unsigned int i=0; i<n; ++i) {
               result += str;
           }
           return result;
       }

       void formatAndPrint(const std::string& key, const std::string& valStr, bool borders) {

           std::string border = borders ? "│" : ""; 

           if (key.empty()) {
               int padding = (WIDTH - (int)valStr.length()) / 2;
               std::cout << border << repeat(" ", padding) << valStr << repeat(" ",padding) << border << "\n";
           } else {
               std::string left = " " + key + repeat(" ", std::max(20-(int)key.length(), 0)) + ": ";
               if ((int)valStr.length() + (int)left.length() < WIDTH - 2) {
                   int padding = WIDTH - (int)left.length() - (int)valStr.length();
                   std::cout << border << left << valStr << repeat(" ", padding) << border << "\n";
               } else {
                   int padding = WIDTH - (int)left.length();
                   std::cout << border << left << repeat(" ", padding) << border << "\n";
                   padding = WIDTH - (int)valStr.length();
                   std::cout << border << valStr << repeat(" ", padding) << border << "\n";
               }
           }
       }

       std::string truncate(std::string str, size_t width) {
           if (str.length() <= width - 2) {
               return str;
           } else if (width < 1) {
               return "";
           } else if (width == 1) {
               return ".";
           }

           return str.substr(0, width - 1) + ".";
       }

       void printHeaderCells(int idx){ (void) idx; std::cout << "\n";}

       template <typename T, typename... Args>
       void printHeaderCells(int idx, T first, Args... args) {
           int w = table_specs[idx].width;
           std::string label = truncate(std::string(first), w-2);

           int padLeft = (w - (int)label.length()) / 2;
           int padRight = w - (int)label.length() - padLeft;

           std::cout << repeat(" ", padLeft) << label << repeat(" ", padRight)<< "│";
           printHeaderCells(idx+1, args...);
       }

       void printCells(int idx) { (void)idx; std::cout << "│\n"; }

       template<typename T, typename... Args>
       void printCells(int idx, T first, Args... args) {
           const auto& spec = table_specs[idx];
           std::cout << "│";

           if (spec.type == LogFmt::Scientific) {
               std::cout << std::scientific << std::setprecision(spec.prec);
           } else if (spec.type == LogFmt::Fixed) {
               std::cout << std::fixed << std::setprecision(spec.prec);
           } else if (spec.type == LogFmt::Int) {
               std::cout << std::defaultfloat << std::noshowpoint;
           }

           std::cout << std::setw(spec.width) << first;
           printCells(idx+1, args...);
       }
};
