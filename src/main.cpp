#include "cli/CliHandler.hpp"

int main(int argc, char** argv) {
    mede::cli::CliHandler handler;
    return handler.run(argc, argv);
}
