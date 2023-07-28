#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "lib/message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::HangmanRequest;
using message::HangmanResponse;
using message::Hangman;


struct HangmanGame {
    std::string secretWord = "hangman";
    std::string maskedWord = "_______";
    std::vector<char> guessedLetters = {};
    int remainingAttempts = 6;

    void updateMaskedWord() {
        for (char c : secretWord) {
            if (std::find(guessedLetters.begin(), guessedLetters.end(), c) != guessedLetters.end()) {
                for (int i = 0; i < secretWord.size(); ++i) {
                    if (secretWord[i] == c) {
                        maskedWord[i] = c;
                    }
                }
            }
        }
    }
};

class HangmanServiceImpl final : public Hangman::Service {
    Status PlayHangman(ServerContext* context, grpc::ServerReaderWriter<message::HangmanResponse, message::HangmanRequest>* stream) override {
        HangmanRequest request;

        while (stream->Read(&request)) {
            
            HangmanResponse response;
            char guess = request.guess().at(0); // İlk karakteri al (kullanıcıdan tek harf alınacağı varsayılır)
            game_.guessedLetters.push_back(guess);

             if (game_.secretWord.find(guess) != std::string::npos) {

                game_.updateMaskedWord();
                response.set_word(game_.maskedWord); // Oyunun güncel kelimesini ayarlayın
                response.set_remaining_attempts(game_.remainingAttempts); // Kalan tahmin hakkını ayarlayın
                response.set_game_over(game_.maskedWord.find('_') == std::string::npos); // Oyunun bitip bitmediğini belirtin              
            } else {
                --game_.remainingAttempts;
                response.set_word(game_.maskedWord); // Oyunun güncel kelimesini ayarlayın
                response.set_remaining_attempts(game_.remainingAttempts); // Kalan tahmin hakkını ayarlayın
                response.set_game_over(game_.remainingAttempts <= 0); // Oyunun bitip bitmediğini belirtin
            }
            if(game_.remainingAttempts <= 0){
                response.set_word(game_.secretWord);
            }

            stream->Write(response);
        }
        return Status::OK;
    }
private:
    HangmanGame game_;
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    HangmanServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
