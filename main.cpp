#include <iostream>
#include <asio.hpp>
#include <thread>
#include <vosk_api.h>
#include <nlohmann/json.hpp>

using namespace asio;

const std::string HOST = "127.0.0.1";
const int PORT = 55555;
io_service service;
ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), PORT));

auto model = vosk_model_new("model");

void handle_client(std::shared_ptr<ip::tcp::socket> client_socket) {
    std::cout << "Подключен клиент" << std::endl;
    auto rec = vosk_recognizer_new(model, 16000);
    std::string prew_res;
    while (true) {
        try {
            std::string res;
            char data[4096];
            int len = client_socket->read_some(buffer(data));
            if (len == 0) {
                break;
            }
                
            if (vosk_recognizer_accept_waveform(rec, data, len))
            {
                auto json = nlohmann::json::parse(vosk_recognizer_result(rec));
                res = json["text"].get<std::string>();
                if (res.empty()) continue;
                res = "result:" + res;
            }
            else{
                auto json = nlohmann::json::parse(vosk_recognizer_partial_result(rec));
                res = json["partial"].get<std::string>();
                if (res.empty() || res == prew_res) continue;
                else {
                    prew_res = res;
                    res = "partial:" + res;
                };
            }
            write(*client_socket, buffer(res));
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка при обработке клиента: " << e.what() << std::endl;
            break;
        }
    }
    client_socket->close();
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "Сервер запущен на " << HOST << ":" << PORT << "..." << std::endl;
    try {
        while (true) {
            std::shared_ptr<ip::tcp::socket> client_socket = std::make_shared<ip::tcp::socket>(service);
            acceptor.accept(*client_socket);
            std::cout << "Подключение от " << client_socket->remote_endpoint().address().to_string() << ":" << client_socket->remote_endpoint().port() << std::endl;
            std::thread client_thread(handle_client, client_socket);
            client_thread.detach();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при ожидании подключения клиента: " << e.what() << std::endl;
      }
  return 0;
}
      