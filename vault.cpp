#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <climits>
#include <fstream>
#include <string>

#include "sha.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"   
#include "ftxui/component/component_base.hpp"  
#include "ftxui/component/component_options.hpp" 
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/util/ref.hpp"

int main() { 
    using namespace ftxui;
    
    auto screen = ScreenInteractive::TerminalOutput();
    std::fstream hashes("hashes.txt");
    std::string master_password_hash = "";
    std::string password_attempt;
    InputOption password_option;
    password_option.password = true;
    password_option.multiline = false;
    Component input_password = Input(&password_attempt, password_option);
    bool logged_in = false;
    bool attempt_failed = false;

    auto input_password_component = Container::Vertical({
        input_password
    });
         
    auto renderer = Renderer(input_password_component, [&] { 
        hashes.clear(); 
        hashes.seekg(0, std::ios::beg);
        if (!std::getline(hashes, master_password_hash)) {
            return vbox({
                hbox(
                    text("Set the vault's master password: "),
                    input_password->Render()
                )
            }) | border;
        }

        if (logged_in) {
            return text("logged in bruh");
        } else {
            return vbox({
                hbox(
                    text(attempt_failed ? "Wrong password, try again:" : "Password: "),
                    input_password->Render()
                )
            }) | border;
        }
    });

    renderer |= CatchEvent([&](Event event) {
        hashes.clear();
        hashes.seekg(0, std::ios::beg);
        if (event == Event::Return) {
            if (!std::getline(hashes, master_password_hash)) { 
                hashes << sha256(password_attempt) << '\n'; 
                master_password_hash = sha256(password_attempt);
                password_attempt.clear();
            } else if (sha256(password_attempt) != master_password_hash) {
                password_attempt.clear();
                attempt_failed = true;
                return true;
            } else {
                logged_in = true;
                return true;
            }
        } else if (event == Event::CtrlD) {
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });
 
    screen.Loop(renderer);
    hashes.close();
}
