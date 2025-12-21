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

class Entry {
    private:
        std::string counter;
        std::string app_name;
        std::string username;
        std::string password;
    
    public:
        Entry(std::string c, std::string a, std::string u, std::string p) : counter(c), app_name(a), username(u), password(p) {};

        std::string get_counter() {return counter;}
        std::string get_app_name() {return app_name;}
        std::string get_username() {return username;}
        std::string get_password() {return password;}
        std::string get_next_counter() {
            unsigned long long num_blocks = std::ceil(app_name.size()/16.0) + std::ceil(username.size()/16.0) + std::ceil(password.size()/16.0);
            std::string next = counter;
            for (unsigned long long i = 0; i < num_blocks; i++) {
                next = increment_hex_string(next);    
            }
            return next;
        }
};

int main() { 
    using namespace ftxui;
    auto screen = ScreenInteractive::TerminalOutput();
    std::vector<Entry*> entries;
    Component entries_container = Container::Vertical({}); 
    bool entries_fetched = false;
    auto refresh_entries = [&] {
        entries_container->DetachAllChildren();
        for (auto& e : entries) {
            entries_container->Add(Button("delete", [&] {
                for (unsigned long i = 0; i < entries.size(); i++) {
                    if (entries[i]->get_counter() == e->get_counter()) {
                        entries.erase(entries.begin() + i);
                        break; 
                    }
                }             
            })); 
        }       
    };

    std::ofstream crypt_write("/home/caleb/documents/crypt", std::ios::app);
    crypt_write.close(); 
    std::ifstream crypt_read("/home/caleb/documents/crypt");
    
    bool attempt_failed = false;    
    std::string first_line; 
    int selector = !std::getline(crypt_read, first_line) ? 0 : 1;   
    std::string verification_message = "This is the verification message, good luck trying to crack my passwords!";
    std::string counter_start = "00000000000000000000000000000000";
    std::string master_key = "";

    std::string init_password;
    InputOption init_option;
    init_option.multiline = false;
    init_option.on_enter = [&] {
        crypt_write.open("/home/caleb/documents/crypt");
        master_key = pbkdf2(utf8_to_hex(init_password), utf8_to_hex("essalado"), 10000, 256);
        std::string verification_ciphertext = counter_mode_encrypt(master_key, utf8_to_hex(verification_message), counter_start); 
        crypt_write << counter_start << ':' << verification_ciphertext << '\n';
        selector = 2;
        crypt_write.close();
    };
    Component init_input = Input(&init_password, init_option); 

    std::string login_attempt;  
    InputOption login_option;
    login_option.password = true;
    login_option.multiline = false;
    login_option.on_enter = [&] {
        master_key = pbkdf2(utf8_to_hex(login_attempt), utf8_to_hex("essalado"), 10000, 256); 
        std::vector<std::string> first_line_split = split(first_line, ':');
        if (first_line_split.size() != 2) {
            std::cerr << "couldn't split first line of crypt file into 2 chunks\n";
            exit(1);
        }
        if (counter_mode_decrypt(master_key, first_line_split[1], first_line_split[0]) == utf8_to_hex(verification_message)) {
            selector = 2;
        } else {
            attempt_failed = true;
            login_attempt.clear();
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
        std::string latest_counter;
        if (entries.empty()) { 
            crypt_read.clear();
            crypt_read.seekg(0, std::ios::beg);
            std::string line;
            getline(crypt_read, line);
            latest_counter = increment_hex_string(split(line, ':')[0]);
        } else {
            latest_counter = (entries.back())->get_next_counter();
        }

        entries.push_back(new Entry(latest_counter, new_app, new_username, new_password)); 
        refresh_entries();
        new_app.clear();
        new_username.clear();
        new_password.clear();        
    });
 
    Component entries_display = Renderer(entries_container, [&] {
        if (!entries_fetched) {
            entries_fetched = true; 
            crypt_read.clear();
            crypt_read.seekg(0, std::ios::beg);
            std::string line;
            getline(crypt_read, line); //consume and skip the verification ciphertext
            while (getline(crypt_read, line)) {
                std::string app_name = hex_to_utf8(counter_mode_decrypt(master_key, split(line, ':')[1], split(line, ':')[0])); 
                std::string counter = split(line, ':')[0];
                getline(crypt_read, line);
                std::string username = hex_to_utf8(counter_mode_decrypt(master_key, split(line, ':')[1], split(line, ':')[0]));
                getline(crypt_read, line);
                std::string password = hex_to_utf8(counter_mode_decrypt(master_key, split(line, ':')[1], split(line, ':')[0]));
                entries.push_back(new Entry(counter, app_name, username, password));
            }
            refresh_entries();
        }

        std::vector<Element> entries_Elements;
        for (unsigned long i = 0; i < entries.size(); i++) {
            entries_Elements.push_back(
                hbox({
                    vbox({
                        text("app name: " + entries[i]->get_app_name()),
                        text("username: " + entries[i]->get_username()),
                        text("password: " + entries[i]->get_password()),         
                    }),
                    entries_container->ChildAt(i)->Render(),
                }) | border
            );
        }
        return vbox(entries_Elements);
    });
    
    Component dashboard_tree = Container::Vertical({
        new_app_input,
        new_username_input,
        new_password_input,        
        entry_submit,
        entries_display,
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
            entries_display->Render(),    
        }) | border;
    });
    
    Component main_container = Container::Tab({
        init,
        login,
        dashboard_renderer,
    }, &selector);
    
    main_container |= CatchEvent([&](Event event) {
        if (event == Event::CtrlD) {
            crypt_read.clear();
            crypt_read.seekg(0, std::ios::beg);
            std::string first_line;
            getline(crypt_read, first_line);
            crypt_write.open("/home/caleb/documents/crypt", std::ios::trunc);
            crypt_write << first_line << '\n';
            for (auto &e : entries) {
                std::string cur_counter = e->get_counter();
                crypt_write << cur_counter << ':' << counter_mode_encrypt(master_key, utf8_to_hex(e->get_app_name()), cur_counter) << '\n';
                for (int i = 0; i < ceil(e->get_app_name().size()/16.0); i++) {
                    cur_counter = increment_hex_string(cur_counter);
                }
                crypt_write << cur_counter << ':' << counter_mode_encrypt(master_key, utf8_to_hex(e->get_username()), cur_counter) << '\n';
                for (int i = 0; i < ceil(e->get_username().size()/16.0); i++) {
                    cur_counter = increment_hex_string(cur_counter);
                }
                crypt_write << cur_counter << ':' << counter_mode_encrypt(master_key, utf8_to_hex(e->get_password()), cur_counter) << '\n';
            }
            crypt_write.close();
            screen.Exit();
            return true;
        }
        return false;
    });
 
    screen.Loop(main_container);
    crypt_read.close();
    std::system("clear");
    std::system("tmux clear-history");
    return 0;
}
