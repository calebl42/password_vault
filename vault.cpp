#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>

#include "primitives/pbkdf2.h"
#include "primitives/aes.h"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"   
#include "ftxui/component/component_base.hpp"  
#include "ftxui/component/component_options.hpp" 
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/util/ref.hpp"

int main() { 
    using namespace ftxui;

    std::ofstream file_creator;
    file_creator.open("/home/caleb/documents/crypt", std::ios::app);
    file_creator.close();
    
    std::fstream crypt;
    crypt.open("/home/caleb/documents/crypt", std::ios::in | std::ios::out | std::ios::app);
    std::string auth_test;
    std::string password_attempt;
       
    auto screen = ScreenInteractive::TerminalOutput();
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
        crypt.clear(); 
        crypt.seekg(0, std::ios::beg);
        if (!std::getline(crypt, auth_test)) {
            return vbox({
                text("It looks like you're logging in for the first time!") | bold,
                text("Set your vault's Master Password: "),
                input_password->Render(),
            }) | border;
        } else if (!logged_in) {
            return vbox({
                text(attempt_failed ? "Wrong password, try again:" : "Master Password: "),
                input_password->Render(),
            }) | border;
        } else {
            return text("You're in!") | bold;
        }
    });
    
    renderer |= CatchEvent([&](Event event) {
        crypt.clear();
        crypt.seekg(0, std::ios::beg);
        std::string verification_message = "This is the verification message, good luck trying to crack my passwords!";
        if (event == Event::Return && !logged_in) {
            if (!std::getline(crypt, auth_test)) {
                crypt.clear();
                crypt.seekg(0, std::ios::beg);
                std::string true_password = pbkdf2(&hmac_sha256, password_attempt, "testing!", 10000, 256);
                std::string verification_ciphertext = counter_mode_encrypt(true_password, verification_message, "AAAAAAAAAAAAAAAA"); 
                crypt << verification_ciphertext << '\n';
                password_attempt.clear();
                logged_in = true;
            } else {
                std::string derived = pbkdf2(&hmac_sha256, password_attempt, "testing!", 10000, 256);
                if (counter_mode_decrypt(derived, auth_test, "AAAAAAAAAAAAAAAA") == verification_message) {
                    password_attempt.clear();
                    logged_in = true; 
                } else {
                    password_attempt.clear();
                    attempt_failed = true;
                }
            }
            return true;
        } else if (event == Event::CtrlD) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
    crypt.close();
    return 0;
}
