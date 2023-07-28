#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "lib/message.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::HangmanRequest;
using message::HangmanResponse;
using message::Hangman;
using grpc::ClientReader;
using grpc::ClientWriter;

void greet()
{
  std::cout << "=====================\n";
  std::cout << "Hangman: The Game\n";
  std::cout << "=====================\n";
  std::cout << "Instructions: Save your friend from being hanged by guessing the letters in the codeword.\n";
}
void display_misses(int misses)
{
  if(misses==0)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==1)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==2)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<"  |   | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==3)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<" /|   | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==4)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<" /|\\  | \n";
    std::cout<<"      | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==5)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<" /|\\  | \n";
    std::cout<<" /    | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }
  else if(misses==6)
  {
    std::cout<<"  +---+ \n";
    std::cout<<"  |   | \n";
    std::cout<<"  O   | \n";
    std::cout<<" /|\\  | \n";
    std::cout<<" / \\  | \n";
    std::cout<<"      | \n";
    std::cout<<" ========= \n";
  }

}

class HangmanClient {
public:
    HangmanClient(std::shared_ptr<Channel> channel) : stub_(Hangman::NewStub(channel)) {}

    // Oyunu fonksiyonu
    void PlayHangman() {
        HangmanRequest request;
        HangmanResponse response;
        ClientContext context;
        std::shared_ptr<grpc::ClientReaderWriter<message::HangmanRequest, message::HangmanResponse>> stream(stub_->PlayHangman(&context));

        display_misses(0);

        while (true) {
            // Oyuncudan tahmini alma          
            std::string guess;
            std::cout << "Enter your guess: ";
            std::cin >> guess;


            request.set_guess(guess);
            stream->Write(request);

            stream->Read(&response);

            if (response.game_over()) {
                if (response.remaining_attempts()== 0) {
                    display_misses(6);
                    std::cout << "Game over! The word was: " << response.word() << std::endl;
                } else if(response.word().find('_') == std::string::npos){
                    std::cout << "Congratulations! You've guessed the word: " << response.word() << std::endl;                
                }
                break;
            }
            // Oyun durumunu        
            display_misses(6 - response.remaining_attempts());
            std::cout << "Word: " << response.word() << "  (Remaining attempts: " << response.remaining_attempts() << ")"<< std::endl;
            // Oyun durumu kontrolü ve diğer işlemler           
        }

        stream->WritesDone();
        Status status = stream->Finish();
    }

private:
    std::unique_ptr<Hangman::Stub> stub_;
};



int main() {
    HangmanClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    greet();
    client.PlayHangman();
    return 0;
}
