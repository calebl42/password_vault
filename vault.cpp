#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

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

class entry {
    std::string app_name;
    std::string username;
    std::string password;
};

int main() { 
    using namespace ftxui;
    auto screen = ScreenInteractive::TerminalOutput();

    std::ofstream file_creator("/home/caleb/documents/crypt", std::ios::app);
    file_creator.close(); 
    std::fstream crypt;
    crypt.open("/home/caleb/documents/crypt", std::ios::in | std::ios::out | std::ios::app);

    bool attempt_failed = false;    
    std::string first_line; 
    int selector = !std::getline(crypt, first_line) ? 0 : 1;   
    std::string verification_message = "This is the verification message, good luck trying to crack my passwords!";
    std::string counter_start = "00000000000000000000000000000000";
    std::string true_password = "";

    std::string init_password;
    InputOption init_option;
    init_option.multiline = false;
    init_option.on_enter = [&] {
        crypt.clear();
        crypt.seekg(0, std::ios::beg);        
        true_password = pbkdf2(utf8_to_hex(init_password), utf8_to_hex("essalado"), 10000, 256);
        std::string verification_ciphertext = counter_mode_encrypt(true_password, utf8_to_hex(verification_message), counter_start); 
        crypt << counter_start << ':' << verification_ciphertext << '\n';
        selector = 2;
    };
    Component init_input = Input(&init_password, init_option); 

    std::string login_attempt;  
    InputOption login_option;
    login_option.password = true;
    login_option.multiline = false;
    login_option.on_enter = [&] {
        true_password = pbkdf2(utf8_to_hex(login_attempt), utf8_to_hex("essalado"), 10000, 256); 
        std::vector<std::string> first_line_split = split(first_line, ':');
        if (first_line_split.size() != 2) {
            std::cerr << "couldn't split first line of crypt file into 2 chunks\n";
            exit(1);
        }
        if (counter_mode_decrypt(true_password, first_line_split[1], first_line_split[0]) == utf8_to_hex(verification_message)) {
            selector = 2;
        } else {
            attempt_failed = true;
        }
    };
    Component login_input = Input(&login_attempt, login_option);

    std::string new_app;
    InputOption new_app_option;
    new_app_option.multiline = false;
    std::string new_username;
    InputOption new_username_option;
    new_username_option.multiline = false;
    std::string new_password;
    InputOption new_password_option;
    new_password_option.multiline = false;
    Component new_app_input = Input(&new_app, new_app_option);    
    Component new_username_input = Input(&new_username, new_username_option);    
    Component new_password_input = Input(&new_password, new_password_option);  

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

    Component entry_submit = Button("submit", [&] {
       // crypt.clear();
       // crypt.seekg(0, std::ios::end);
       // std::string lastline;
       // while (getline(crypt, lastline));
       // std::vector<std::string> lastline_split = split(lastline, ':');
       // std::string latest_counter = lastline_split[0];

       // crypt.clear();
       // crypt.seekg(0, std::ios::end);
       // for (int i = 0; i < ceil(lastline_split[1].size()/16.0)+1; i++) latest_counter = increment_hex_string(latest_counter); 
       // crypt << latest_counter << ':' << counter_mode_encrypt(true_password, utf8_to_hex(new_app), latest_counter) << '\n';
       // for (int i = 0; i < ceil(new_app.size()/16.0)+1; i++) latest_counter = increment_hex_string(latest_counter);
       // crypt << latest_counter << ':' << counter_mode_encrypt(true_password, utf8_to_hex(new_username), latest_counter) << '\n';
       // for (int i = 0; i < ceil(new_username.size()/16.0)+1; i++) latest_counter = increment_hex_string(latest_counter);
       // crypt << latest_counter << ':' << counter_mode_encrypt(true_password, utf8_to_hex(new_password), latest_counter) << '\n';
        
        new_app.clear();
        new_username.clear();
        new_password.clear();        
    });
    
    Component dashboard_tree = Container::Vertical({
        new_app_input,
        new_username_input,
        new_password_input,        
        entry_submit,
    });

    Component dashboard_renderer = Renderer(dashboard_tree, [&]{
        return vbox({
            vbox({
                text("New entry") | bold, 
                hbox({
                    text("application name: "), new_app_input->Render(),
                }),
                hbox({
                    text("username/email: "), new_username_input->Render(),
                }), 
                hbox({
                    text("password: "), new_password_input->Render(),
                }),
                entry_submit->Render(), 
            }) | border,
            vbox({
                text("jamming with the hamming"),
            }) | border,
        }) | border;
    });
    
    Component main_container = Container::Tab({
        init,
        login,
        dashboard_renderer,
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
    std::system("clear");
    std::system("tmux clear-history");
    return 0;
}
