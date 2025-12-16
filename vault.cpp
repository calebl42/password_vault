#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
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
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/color.hpp"


int main() { 
    using namespace ftxui;
    auto screen = ScreenInteractive::TerminalOutput();

    std::ofstream file_creator("/home/caleb/documents/crypt", std::ios::app);
    file_creator.close(); 
    std::fstream crypt;
    crypt.open("/home/caleb/documents/crypt", std::ios::in | std::ios::out | std::ios::app);

    bool attempt_failed = false;    
    std::string auth_test; 
    int selector = !std::getline(crypt, auth_test) ? 0 : 1;   
    std::string verification_message = "This is the verification message, good luck trying to crack my passwords!";
    
    std::string init_password;
    InputOption init_option;
    init_option.multiline = false;
    init_option.on_enter = [&] {
        crypt.clear();
        crypt.seekg(0, std::ios::beg);
        std::string true_password = pbkdf2(&hmac_sha256, init_password, "testing!", 10000, 256);
        std::string verification_ciphertext = counter_mode_encrypt(true_password, verification_message, "AAAAAAAAAAAAAAAA"); 
        crypt << verification_ciphertext << '\n';
        init_password.clear();
        selector = 2;
    };
    Component init_input = Input(&init_password, init_option); 

    std::string login_attempt;  
    InputOption login_option;
    login_option.password = true;
    login_option.multiline = false;
    login_option.on_enter = [&] {
        crypt.clear();
        crypt.seekg(0, std::ios::beg);
        std::string derived = pbkdf2(&hmac_sha256, login_attempt, "testing!", 10000, 256);
        login_attempt.clear();
        if (counter_mode_decrypt(derived, auth_test, "AAAAAAAAAAAAAAAA") == verification_message) {
            selector = 2;
        } else {
            attempt_failed = true;
        }
    };
    Component login_input = Input(&login_attempt, login_option);

    std::string entry_name;
    InputOption new_entry_option;
    Component new_entry = Input(&entry_name, new_entry_option);    

    Component init = Renderer(init_input, [&]{
        return vbox({
            text("It looks like you're logging in for the first time!") | bold,
            hbox({
                text("Set your vault's Master Password: "),
                init_input->Render(),
            }),
        }) | border;
    });
    
    Component login = Renderer(login_input, [&]{
        return vbox({
            text(attempt_failed ? "Wrong password, try again:" : "Master Password: "),
            login_input->Render(),
        }) | border;
    });
        
    Component dashboard = Renderer(new_entry, [&]{
        return vbox({
            hbox({
                text("Add new password"),
                new_entry->Render(),
            }),
        }) | border;
    });
    
    Component main_container = Container::Tab({
        init,
        login,
        dashboard,
    }, &selector);
    
    main_container |= CatchEvent([&](Event event) {
        if (event == Event::CtrlD) {
            screen.Exit();
            return true;
        }
        return false;
    });
 
    screen.Loop(main_container);
    crypt.close();
    return 0;
}
