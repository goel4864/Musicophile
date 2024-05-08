#include <SFML/Graphics.hpp> // SFML graphics library
#include <iostream> // Input and output stream
#include <thread> // Threading support
#include <chrono> // Time utilities
#include <string> // String manipulation
#include <cmath> // Mathematical functions
#include <mysql_connection.h> // MySQL database connection
#include <mysql_driver.h> // MySQL driver
#include <mysql_error.h> // MySQL error handling
#include <cppconn/statement.h> // MySQL statement
#include <cppconn/prepared_statement.h> // MySQL prepared statement
#include <vector> // Vector container
#include <memory> // Smart pointers
#include <cstring> // String manipulation functions
#include <sys/socket.h> // Socket programming
#include <arpa/inet.h> // Internet address manipulation
#include <unistd.h> // POSIX API access
#include "../network.h" // Custom network module
#include <atomic> // Atomic operations
#include <stdio.h> // Standard input and output
#include <stdlib.h> // Standard library functions
#include <fstream> // File stream operations
#include "audio.h" // Audio processing module
#include <future> // Asynchronous execution support
#include "client.h"



using namespace std;


namespace UI
{
    
const int POINTS = 10;

// Utility functions
static int HexadecimalToDecimal(std::string hex) {
    int hexLength = hex.length();
    double dec = 0;

    for (int i = 0; i < hexLength; ++i) {
        char b = hex[i];

        if (b >= 48 && b <= 57)
            b -= 48;
        else if (b >= 65 && b <= 70)
            b -= 55;

        dec += b * pow(16, ((hexLength - i) - 1));
    }

    return (int)dec;
}



class RoundedRectangle : public sf::ConvexShape {
public:
    RoundedRectangle(float x=0, float y=0, float rectWidth=0, float rectHeight=0, float radius=0, float Outline=0) {
        setPointCount(4 * POINTS);
        setOutlineThickness(Outline);

        float X = 0, Y = 0;
        for (int i = 0; i < POINTS; i++) {
            X += radius / POINTS;
            Y = sqrt(radius * radius - X * X);
            setPoint(i, sf::Vector2f(X + x + rectWidth - radius, y - Y + radius));
        }
        Y = 0;
        for (int i = 0; i < POINTS; i++) {
            Y += radius / POINTS;
            X = sqrt(radius * radius - Y * Y);
            setPoint(i + POINTS, sf::Vector2f(x + rectWidth + X - radius, y + rectHeight - radius + Y));
        }
        X = 0;
        for (int i = 0; i < POINTS; i++) {
            X += radius / POINTS;
            Y = sqrt(radius * radius - X * X);
            setPoint(i + 2 * POINTS, sf::Vector2f(x + radius - X, y + rectHeight - radius + Y));
        }
        Y = 0;
        for (int i = 0; i < POINTS; i++) {
            Y += radius / POINTS;
            X = sqrt(radius * radius - Y * Y);
            setPoint(i + 3 * POINTS, sf::Vector2f(x + radius - X, y + radius - Y));
        }
    }
};

// UI Manager class

class UIManager {

private:
    sf::RenderWindow& window;
    sf::Font font;
    RoundedRectangle usernameBox;
    RoundedRectangle usernameButton;
    RoundedRectangle passwordButton;
    RoundedRectangle confirmPasswordButton;
    // RoundedRectangle playButton;
    sf::Text usernameText;
    sf::Text passwordText;
    sf::Text confirmPasswordText;
    sf::Text usernameButtonText; // Added
    sf::Text passwordButtonText; // 
    sf::Text confirmPasswordButtonText;
    sf::Text loginButtonText; // Added
    sf::ConvexShape loginButton;
    sf::Text SignupButtonText; // Added
    sf::ConvexShape SignupButton;
    sf::ConvexShape toLoginButton;
    sf::Text toLoginButtonText;
    sf::Text welcomeText;
    std::string inputUserName;
    std::string inputPassword;
    std::string inputConfirmPassword;
    std::string actualPassword;
    std::string displayedPassword;
    std::string actualConfirmPassword;
    std::string displayedConfirmPassword;
    sf::Color topColor;
    sf::Color bottomColor;
    sf::RectangleShape leftRect;
    sf::RectangleShape rightRect;
    std::vector<sf::Text> leftTexts;
    std::vector<sf::Sprite> sprites;
    std::vector<std::shared_ptr<sf::Texture>> textures;
    sf::Sprite logoSprite;
    sf::Sprite playButton;
    sf::Sprite forwardButton;
    sf::Sprite backwardButton;
    sf::Sprite pauseButton;
    sf::Sprite resumeButton;
    sf::Sprite eyeButton;
    sf::Sprite confirmEyeButton;
    sf::Sprite quitButton;
    std::shared_ptr<sf::Texture> quitTexture =std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> forwardTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> playTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> logoTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> backwardTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> pauseTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> resumeTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> eyeTexture = std::make_shared<sf::Texture>();
    std::shared_ptr<sf::Texture> confirmeyeTexture = std::make_shared<sf::Texture>();
    
public:
    static int clickedIndex; 
    static bool resetui;
    static bool nextMainPage;
    static bool isEnteringUsername;
    static bool isEnteringPassword;
    static bool isEnteringConfirmPassword;
    static bool checkResetUI;
    static bool checkMainPageUI;
    static std::string selectedGenere;
    static bool checkResetMainPage;
    static bool genereSelectedPage;
    static bool playedSong;
    static bool clickSongButton;
    static bool checkPlaySongPage;
    static bool pausedSong;
    static bool resumeSong;
    static bool forwardSong;
    static bool backwardSong;
    static bool startPage;
    static bool showPassword;
    static bool quitWindow;
    UIManager(sf::RenderWindow& win,std::map<std::string, std::vector<std::string>> &genreImagePathMap) : window(win),topColor(255, 120, 1), bottomColor(219, 45, 238) {
        if (!font.loadFromFile("..//media//fonts//arial.ttf")) {
            std::cerr << "Error: Font file not found or failed to load" << std::endl;
        }
    }

        std::string extractFilename(const std::string& fullPath) {
            // Find the position of the last '/' character
            size_t lastSlashPos = fullPath.find_last_of('/');
            
            // If '/' is found, add 1 to move to the character after '/'
            // If '/' is not found, start from the beginning of the string
            size_t startPos = (lastSlashPos != std::string::npos) ? lastSlashPos + 1 : 0;

            // Find the position of the '.' character indicating the file extension
            size_t extensionPos = fullPath.find_last_of('.');

            // Extract the substring between the last '/' and '.'
            return fullPath.substr(startPos, extensionPos - startPos);
        }


    void initUIElements() {
        // Set up username box
        usernameBox = RoundedRectangle(650, 100, 500, 800, 20, 2);

        // Set up username button
        usernameButton = RoundedRectangle(704, 340, 400, 50, 10, 1);
        usernameButton.setFillColor(hexToColor("#F2F2F2"));

        //Set up usernameButtonText Button
        usernameButtonText.setFont(font);
        usernameButtonText.setCharacterSize(20);
        usernameButtonText.setFillColor(sf::Color::Black);
        usernameButtonText.setString("Username");
        usernameButtonText.setPosition(705, 310);

        // Set up password button
        passwordButton = RoundedRectangle(704, 450, 400, 50, 10, 1);
        passwordButton.setFillColor(hexToColor("#F2F2F2"));

        //Set up passwordButtonText Button 
        passwordButtonText.setFont(font);
        passwordButtonText.setCharacterSize(20);
        passwordButtonText.setString("Password");
        passwordButtonText.setPosition(710, 420);
        passwordButtonText.setFillColor(sf::Color::Black);

        if (!eyeTexture->loadFromFile("..//media//images//logo//eye.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        eyeButton.setTexture(*eyeTexture);

        // Calculate position of logo sprite
        sf::Texture& eyeTextureRef = *eyeTexture;

        eyeButton.setPosition(1060, 460);

        


         // Set up password button
        confirmPasswordButton = RoundedRectangle(704, 560, 400, 50, 10, 1);
        confirmPasswordButton.setFillColor(hexToColor("#F2F2F2"));
        
        //Set up passwordButtonText Button 
        confirmPasswordButtonText.setFont(font);
        confirmPasswordButtonText.setCharacterSize(20);
        confirmPasswordButtonText.setString("Confirm Password");
        confirmPasswordButtonText.setPosition(710, 530);
        confirmPasswordButtonText.setFillColor(sf::Color::Black);


        if (!confirmeyeTexture->loadFromFile("..//media//images//logo//eye.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        confirmEyeButton.setTexture(*confirmeyeTexture);

        // Calculate position of logo sprite
        sf::Texture& confirmeyeTextureRef = *confirmeyeTexture;

        confirmEyeButton.setPosition(1060, 570);
    
        // Set up Signup  button
        SignupButton = RoundedRectangle(790, 660, 200, 50, 10, 1);
        SignupButton.setFillColor(sf::Color(173, 216, 230));

        //Set up SignupButtonText 

        SignupButtonText.setFont(font);
        SignupButtonText.setCharacterSize(20);
        SignupButtonText.setFillColor(sf::Color::Black);
        SignupButtonText.setString("Sign Up ");
        SignupButtonText.setPosition(859, 667);   
        
        //Set up toLoginButton 
        toLoginButton = RoundedRectangle(960, 773, 120, 40, 10, 1);
        toLoginButton.setFillColor(sf::Color(173, 216, 230));

        //Set up toLoginButtonText  
        toLoginButtonText.setFont(font);
        toLoginButtonText.setCharacterSize(20);
        toLoginButtonText.setFillColor(sf::Color::Black);
        toLoginButtonText.setString("Already Have an Account ? Log In Here ");
        toLoginButtonText.setPosition(715, 780); 

        // Set up texts
        usernameText.setFont(font);
        usernameText.setCharacterSize(20);
        usernameText.setFillColor(sf::Color::Black);
        usernameText.setString("");
        usernameText.setPosition(715, 360);

        passwordText.setFont(font);
        passwordText.setCharacterSize(20);
        passwordText.setFillColor(sf::Color::Black);
        passwordText.setString("");
        passwordText.setPosition(715, 470);

        confirmPasswordText.setFont(font);
        confirmPasswordText.setCharacterSize(20);
        confirmPasswordText.setFillColor(sf::Color::Black);
        confirmPasswordText.setString("");
        confirmPasswordText.setPosition(715, 580);

        welcomeText.setFont(font);
        welcomeText.setCharacterSize(50);
        welcomeText.setFillColor(sf::Color(28, 22, 120));
        welcomeText.setString("Welcome");
        welcomeText.setPosition(790, 200);   

        if (!logoTexture->loadFromFile("..//media//images//logo//logo.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        
        //  sf::Sprite sprite(*logoTexture);
        logoSprite.setTexture(*logoTexture);

        // Calculate position of logo sprite
        sf::Texture& logoTextureRef = *logoTexture;

        logoSprite.setPosition(947, 120);

        float targetWidth = 200.0f; // Adjust as needed
        float targetHeight = 40.0f; // Adjust as needed
        logoSprite.setScale(targetWidth / logoTextureRef.getSize().x, targetHeight / logoTextureRef.getSize().y);

    }

     void nextLoginUIElements() {
        // Set up username box
        usernameBox = RoundedRectangle(650, 100, 500, 800, 20, 2);

        // Set up username button
        usernameButton = RoundedRectangle(704, 340, 400, 50, 10, 1);
        usernameButton.setFillColor(hexToColor("#F2F2F2"));

        //Set up usernameButtonText Button
        usernameButtonText.setFont(font);
        usernameButtonText.setCharacterSize(20);
        usernameButtonText.setFillColor(sf::Color::Black);
        usernameButtonText.setString("Username");
        usernameButtonText.setPosition(705, 310);

        // Set up password button
        passwordButton = RoundedRectangle(704, 480, 400, 50, 10, 1);
        passwordButton.setFillColor(hexToColor("#F2F2F2"));

        //Set up passwordButtonText Button 
        passwordButtonText.setFont(font);
        passwordButtonText.setCharacterSize(20);
        passwordButtonText.setString("Password");
        passwordButtonText.setPosition(710, 450);
        passwordButtonText.setFillColor(sf::Color::Black);


        if (!eyeTexture->loadFromFile("..//media//images//logo//eye.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        eyeButton.setTexture(*eyeTexture);

        // Calculate position of logo sprite
        sf::Texture& eyeTextureRef = *eyeTexture;

        eyeButton.setPosition(1060, 490);

        
        // Set up login button
        loginButton = RoundedRectangle(790, 660, 200, 50, 10, 1);
        loginButton.setFillColor(sf::Color(173, 216, 230));

        //Set up loginButtonText Button

        loginButtonText.setFont(font);
        loginButtonText.setCharacterSize(20);
        loginButtonText.setFillColor(sf::Color::Black);
        loginButtonText.setString("Sign in ");
        loginButtonText.setPosition(859, 667);   

        // Set up texts
        usernameText.setFont(font);
        usernameText.setCharacterSize(20);
        usernameText.setFillColor(sf::Color::Black);
        usernameText.setString("");
        usernameText.setPosition(715, 360);

        passwordText.setFont(font);
        passwordText.setCharacterSize(20);
        passwordText.setFillColor(sf::Color::Black);
        passwordText.setString("");
        passwordText.setPosition(715, 500);     

        welcomeText.setFont(font);
        welcomeText.setCharacterSize(50);
        welcomeText.setFillColor(sf::Color(28, 22, 120));
        welcomeText.setString("Welcome");
        welcomeText.setPosition(790, 200);   

        if (!logoTexture->loadFromFile("..//media//images//logo//logo.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        
        logoSprite.setTexture(*logoTexture);

        // Calculate position of logo sprite
        sf::Texture& logoTextureRef = *logoTexture;

        logoSprite.setPosition(947, 120);

        float targetWidth = 200.0f; // Adjust as needed
        float targetHeight = 40.0f; // Adjust as needed
        logoSprite.setScale(targetWidth / logoTextureRef.getSize().x, targetHeight / logoTextureRef.getSize().y);


        

    }
   

     void resetMainPageElements(std::map<std::string, std::vector<std::string>> &genreImagePathMap){
           leftRect.setSize(sf::Vector2f(window.getSize().x * 0.2, window.getSize().y));
        leftRect.setFillColor(sf::Color::Black);

        // Define the right rectangle (80% of the width)
        rightRect.setSize(sf::Vector2f(window.getSize().x * 0.8, window.getSize().y));
        rightRect.setPosition(window.getSize().x * 0.2, 0); // Position it to the right of the left rectangle
        rightRect.setFillColor(sf::Color(251, 234, 128));

        // Load logo texture
        if (!logoTexture->loadFromFile("..//media//images//logo//logo.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        logoSprite.setTexture(*logoTexture);

        // Calculate position of logo sprite
        sf::Texture& logoTextureRef = *logoTexture;

        logoSprite.setPosition(1600, 950);

        // Scale the logo sprite
        float targetWidth = 250.0f; // Adjust as needed
        float targetHeight = 50.0f; // Adjust as needed
        logoSprite.setScale(targetWidth / logoTextureRef.getSize().x, targetHeight / logoTextureRef.getSize().y);

        // Create text objects for left topics
       
        if (!font.loadFromFile("..//media//fonts//arial.ttf")) {
            std::cerr << "Error: Font file not found or failed to load" << std::endl;
            // Handle error
        }

        std::vector<std::string> leftTopics = {
            "Bollywood","Disco","Hip_Hop","Punjabi","Rap"
        };

        float leftTextStartX = 30; // Starting X position for left topics
        float leftTextStartY = 50; // Starting Y position for left topics
        leftTexts.clear();
        for (const auto& topic : leftTopics) {
            sf::Text bulletPoint("• ", font, 15); // Bullet point
            bulletPoint.setPosition(leftTextStartX, leftTextStartY + leftTexts.size() * 30); // Adjust position as needed

            sf::Text text(" " + topic, font, 20); // Topic text with space at the beginning
            text.setPosition(leftTextStartX + bulletPoint.getLocalBounds().width, leftTextStartY + leftTexts.size() * 30); // Adjust position as needed

            leftTexts.push_back(bulletPoint);
            leftTexts.push_back(text);
        }

  
        sprites.clear();
        textures.clear();

        auto it = genreImagePathMap.find(selectedGenere);
        if (it != genreImagePathMap.end()) {
        for (const auto& imagePath : it->second) {
            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(imagePath)) {
                std::cerr << "Error: Failed to load image " << imagePath << std::endl;
                // Handle error
                return;
            }
            sf::Sprite sprite(*texture);
            textures.push_back(texture);
            sprites.push_back(sprite);
        }
    }

    arrangeSprites();
    }
    
      void playSongPageElements(std::map<std::string, std::vector<std::string>> &genreImagePathMap,std::string &clickedSongPage){

        
        if (!playTexture->loadFromFile("..//media//images//logo//player.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        playButton.setTexture(*playTexture);

        // Calculate position of logo sprite
        sf::Texture& playTextureRef = *playTexture;

        playButton.setPosition(900, 315);
        
        
        if (!pauseTexture->loadFromFile("..//media//images//logo//pauseButton.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        pauseButton.setTexture(*pauseTexture);

        // Calculate position of logo sprite
        sf::Texture& pauseTextureRef = *pauseTexture;

        pauseButton.setPosition(970, 315);

         if (!resumeTexture->loadFromFile("..//media//images//logo//resume.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        resumeButton.setTexture(*resumeTexture);

        // Calculate position of logo sprite
        sf::Texture& resumeTextureRef = *resumeTexture;

        resumeButton.setPosition(1040, 315);
       
        if (!backwardTexture->loadFromFile("..//media//images//logo//backwardButton.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        backwardButton.setTexture(*backwardTexture);

        // Calculate position of logo sprite
        sf::Texture& backwardTextureRef = *backwardTexture;

        backwardButton.setPosition(1110, 315);

        if (!forwardTexture->loadFromFile("..//media//images//logo//forwardButton.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        forwardButton.setTexture(*forwardTexture);

        // Calculate position of logo sprite
        sf::Texture& forwardTextureRef = *forwardTexture;

        forwardButton.setPosition(1180, 315);


        if (!quitTexture->loadFromFile("..//media//images//logo//quitButton.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        quitButton.setTexture(*quitTexture);

        // Calculate position of logo sprite
        sf::Texture& quitTextureRef = *quitTexture;

        quitButton.setPosition(1770, 10);
        
        
        leftRect.setSize(sf::Vector2f(window.getSize().x * 0.2, window.getSize().y));
        leftRect.setFillColor(sf::Color::Black);

        // Define the right rectangle (80% of the width)
        rightRect.setSize(sf::Vector2f(window.getSize().x * 0.8, window.getSize().y));
        rightRect.setPosition(window.getSize().x * 0.2, 0); // Position it to the right of the left rectangle
        rightRect.setFillColor(sf::Color(253, 250, 199));

        // Load logo texture
        if (!logoTexture->loadFromFile("..//media//images//logo//logo.png")) {
            std::cerr << "Error: Failed to load image" << std::endl;
            // Handle error
        }
        logoSprite.setTexture(*logoTexture);

        // Calculate position of logo sprite
        sf::Texture& logoTextureRef = *logoTexture;

        logoSprite.setPosition(1600, 950);

        // Scale the logo sprite
        float targetWidth = 250.0f; // Adjust as needed
        float targetHeight = 50.0f; // Adjust as needed
        logoSprite.setScale(targetWidth / logoTextureRef.getSize().x, targetHeight / logoTextureRef.getSize().y);

        // Create text objects for left topics
       
        if (!font.loadFromFile("..//media//fonts//arial.ttf")) {
            std::cerr << "Error: Font file not found or failed to load" << std::endl;
            // Handle error
        }

       std::vector<std::string> leftTopics = {
            "Bollywood","Disco","Hip_Hop","Punjabi","Rap"
        };


        float leftTextStartX = 30; // Starting X position for left topics
        float leftTextStartY = 50; // Starting Y position for left topics
        leftTexts.clear();
        for (const auto& topic : leftTopics) {
            sf::Text bulletPoint("• ", font, 15); // Bullet point
            bulletPoint.setPosition(leftTextStartX, leftTextStartY + leftTexts.size() * 30); // Adjust position as needed

            sf::Text text(" " + topic, font, 20); // Topic text with space at the beginning
            text.setPosition(leftTextStartX + bulletPoint.getLocalBounds().width, leftTextStartY + leftTexts.size() * 30); // Adjust position as needed

            leftTexts.push_back(bulletPoint);
            leftTexts.push_back(text);
        }

        sprites.clear();
        textures.clear();

              int index;
         auto it = genreImagePathMap.find(selectedGenere);
    if (it != genreImagePathMap.end()) {
        // for (const auto& imagePath : it->second) { 
            for(int i=0;i<it->second.size();i++){
                if(it->second[i]==clickedSongPage){
                    index=i;
                    UIManager::clickedIndex=i;
                }
            }
            for(int i=0;i<4;i++){ 
                // index=i;
                if(index>=it->second.size()){
                    index=index%it->second.size();
                }
            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(it->second[index])) {
                std::cerr << "Error: Failed to load image " << it->second[index] << std::endl;
                // Handle errorstatic
                return ;
            }
            index++;
            sf::Sprite sprite(*texture);
            textures.push_back(texture);
            sprites.push_back(sprite);
        }
    }

    arrangeSprites2();
    }
      void arrangeSprites2() {
        int col = 0;
        int row = 0;
        sprites[0].setPosition(window.getSize().x * 0.5f, 50);
        float startX = window.getSize().x * 0.2f + 210;

    // Position the remaining sprites horizontally below the first sprite
    for (size_t i = 1; i < sprites.size(); ++i) {
        sprites[i].setPosition(startX + (i - 1) * 380, 570); // Adjust the Y position as needed
    }

    }
    void arrangeSprites() {
        int col = 0;
        int row = 0;
        int startX = window.getSize().x * 0.2 + 100;

        for (size_t i = 0; i < sprites.size(); ++i) {
            sprites[i].setPosition(startX + col * 350, 50 + row * 300);
            row++;
            if (row == 3) {
                col++;
                row = 0;
            }
        }
    }

    void handleEvents(sql::Connection *con, sql::PreparedStatement *pstmt,std::map<std::string, std::vector<std::string>> &genreImagePathMap,ClientSessionData &clientSessionData, std::atomic<bool> &clientRunning, int clientSocket,std::string &requestSongPlay,std::string &clickedSongPage) 
{
        sf::Event event;
        while (window.pollEvent(event)) 
        {
        
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle mouse button pressed
            if (event.type == sf::Event::MouseButtonPressed) 
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

                // Username button clicked
                if (usernameButton.getGlobalBounds().contains(worldPos)) {
                      UIManager::isEnteringUsername = true;
                } else {
                    isEnteringUsername = false;
                }

                // Password button clicked
                if (passwordButton.getGlobalBounds().contains(worldPos)) {
                    UIManager::isEnteringPassword=true;
                } else {
                    isEnteringPassword = false;
                }
                // Confirm Password Clicked
                 if (confirmPasswordButton.getGlobalBounds().contains(worldPos)) {
                    UIManager::isEnteringConfirmPassword=true;
                } else {
                    isEnteringConfirmPassword = false;
                }

                //To Login Button CLicked 
                if(toLoginButton.getGlobalBounds().contains(worldPos)){
                        UIManager::resetui=true;
                        UIManager::checkResetUI=true;
                        
                }
                if (playButton.getGlobalBounds().contains(worldPos)) {
                            requestSongPlay.clear();
                            requestSongPlay+=selectedGenere;
                            requestSongPlay+="//";
                            requestSongPlay+=extractFilename(clickedSongPage);
                            requestSongPlay+=".wav";
                            UIManager::playedSong=true;
                } 
                if(quitButton.getGlobalBounds().contains(worldPos)){
                    clientRunning=false;
                    UIManager::quitWindow=true;
                }

                 if (pauseButton.getGlobalBounds().contains(worldPos)) {
                    
                        UIManager::pausedSong=true;

                } 
                if (resumeButton.getGlobalBounds().contains(worldPos)) {
                    
                        UIManager::resumeSong=true;
                        
                } 
                 if (forwardButton.getGlobalBounds().contains(worldPos)) {
                    
                        // UIManager::playedSong=false;
                        UIManager::forwardSong=true;
                } 
                if (backwardButton.getGlobalBounds().contains(worldPos)) {

                        UIManager::backwardSong=true;
                       
                } 
                if (eyeButton.getGlobalBounds().contains(worldPos)) {
                    passwordText.setString(inputPassword);
                }
                if (confirmEyeButton.getGlobalBounds().contains(worldPos)) {
                    confirmPasswordText.setString(inputConfirmPassword);
                }  

                //Sign Up Button CLicked
                if (SignupButton.getGlobalBounds().contains(worldPos)) 
                {

                     if (!isValidEmailAddress(inputUserName)) {
                            sf::Text invalidUsernameText("Invalid email format! Please enter a valid email address.", font, 20);
                            invalidUsernameText.setPosition(100, 100); // Adjust position as needed

                            // Draw the text
                            window.draw(invalidUsernameText);
                            window.display();

                            // Wait for 1 second
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        } else if (inputPassword.length() < 8) { // Check if password is at least 8 characters long
                            sf::Text invalidPasswordText("Password must be at least 8 characters long.", font, 20);
                            invalidPasswordText.setPosition(100, 150); // Adjust position as needed

                            // Draw the text
                            window.draw(invalidPasswordText);
                            window.display();

                            // Wait for 1 second
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        } 
                        else 
                            if(inputPassword!=inputConfirmPassword){
                                window.display();

                                // Wait for 1 second
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                            } 
                            else 
                            {
                                    pstmt = con->prepareStatement("SELECT * FROM users WHERE emailid = ? AND password = ?");
                                    pstmt->setString(1, inputUserName); // Bind emailid parameter
                                    pstmt->setString(2, inputPassword); // Bind password parameter

                                    // Execute the query
                                    sql::ResultSet *res = pstmt->executeQuery();

                                    // Check if any rows were returned
                                    if (res->next()) {
                                        // User is logged in
                                        std::cout << "User Already Exists , Please Login" << std::endl;
                                    
                                    } 
                                    
                                    else 
                                    {
                                            // New Account Creation
                                            std::cout << "Account Created Successfully" << std::endl;
                                            pstmt = con->prepareStatement("INSERT INTO users (emailid, password) VALUES (?, ?)");

                                            // Bind values to parameters in the SQL statement
                                            pstmt->setString(1, inputUserName); // Bind emailid parameter
                                            pstmt->setString(2, inputPassword); // Bind password parameter

                                            // Execute the insert query
                                            int rowsAffected = pstmt->executeUpdate();

                                            if (rowsAffected > 0) {
                                                std::cout << "User information inserted into the database." << std::endl;
                                            } else {
                                                std::cerr << "Failed to insert user information into the database." << std::endl;
                                            }

                                        UIManager::resetui=true;
                                        UIManager::checkResetUI=true;
                                        inputUserName.clear();
                                        inputPassword.clear();
                                        inputConfirmPassword.clear();
                                    }

                            }
                }


                // Login button clicked
                if (loginButton.getGlobalBounds().contains(worldPos)) 
                {

                     if (!isValidEmailAddress(inputUserName)) {
                            sf::Text invalidUsernameText("Invalid email format! Please enter a valid email address.", font, 20);
                            invalidUsernameText.setPosition(100, 100); // Adjust position as needed

                            // Draw the text
                            window.draw(invalidUsernameText);
                            window.display();

                            // Wait for 1 second
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        } else 
                            if (inputPassword.length() < 8) { 
                                // Check if password is at least 8 characters long
                                sf::Text invalidPasswordText("Password must be at least 8 characters long.", font, 20);
                                invalidPasswordText.setPosition(100, 150); // Adjust position as needed
                                // Draw the text
                                window.draw(invalidPasswordText);
                                window.display();
                                // Wait for 1 second
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                                }
                            else {
                                pstmt = con->prepareStatement("SELECT * FROM users WHERE emailid = ? AND password = ?");
                                pstmt->setString(1, inputUserName); // Bind emailid parameter
                                pstmt->setString(2, inputPassword); // Bind password parameter
                                // Execute the query
                                sql::ResultSet *res = pstmt->executeQuery();


                                // Check if any rows were returned
                                if (res->next()) {
                                    // User is logged in
                                    std::cout << " User Signed in!" << std::endl;
                                    inputUserName.clear();
                                    inputPassword.clear();
                                    UIManager::resetui=false;
                                    UIManager::genereSelectedPage=true;
                                    UIManager::checkResetMainPage=true;
                                } 
                                else {
                                    sf::Text invalidPasswordText(" Invalid email or password" , font, 20);
                                    invalidPasswordText.setPosition(100, 100); // Adjust position as needed

                                    // Draw the text
                                    window.draw(invalidPasswordText);
                                    window.display();

                                    // Wait for 1 second
                                    std::this_thread::sleep_for(std::chrono::seconds(1));
                                }
                            }
                }



                // Check if mouse is over image
                for (size_t i = 0; i < sprites.size(); ++i) {
                    if (sprites[i].getGlobalBounds().contains(worldPos)) {
                        UIManager::clickSongButton=true;
                        UIManager::checkPlaySongPage=true;
                        UIManager::genereSelectedPage=false;
                        
                        if (genreImagePathMap.find(selectedGenere) != genreImagePathMap.end()) 
                        {
                            std::vector<std::string>& imagePaths = genreImagePathMap[selectedGenere];
                            int size=imagePaths.size();
                            clickedSongPage.clear();
                            clickedSongPage = imagePaths[(i+UIManager::clickedIndex)%size] ;
                        }
                    }
                }
                for (size_t i = 0; i < leftTexts.size(); ++i) {
                        if (leftTexts[i].getGlobalBounds().contains(worldPos)) {
                            std::string genreName = leftTexts[i].getString(); // Get the genre name
                            auto it = genreImagePathMap.find(genreName);
                            UIManager::selectedGenere=genreName.substr(1);
                            UIManager::genereSelectedPage=true;
                            UIManager::resetui=false;
                            UIManager::checkResetMainPage=true;
                            }
                }

            }//MouseButtonPressed event handled


            // Handling text input
            if (isEnteringUsername && event.type == sf::Event::TextEntered) {
                //only ascii characters are processed
                if (event.text.unicode < 128) {
                    //backspace key
                    if (event.text.unicode == '\b') { 
                        if (!usernameText.getString().isEmpty())
                            {
                                usernameText.setString(usernameText.getString().substring(0, usernameText.getString().getSize() - 1));
                                if(!inputUserName.empty())
                                {
                                    inputUserName.erase(inputUserName.size()-1);
                                }
                            }   
                    } 
                    else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Enter
                        std::cout << "Username: " << usernameText.getString().toAnsiString() << std::endl;
                        
                    } else {
                        inputUserName+=static_cast<char>(event.text.unicode);
                        // std::cout<<usernameText.getString().toAnsiString()<<std::endl;
                        usernameText.setString(usernameText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            

            if (isEnteringPassword && event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128)
                {
                    if (event.text.unicode == '\b') // Backspace
                    {
                        if (!passwordText.getString().isEmpty())
                            {passwordText.setString(passwordText.getString().substring(0, passwordText.getString().getSize() - 1));
                            if(!inputPassword.empty()){
                            inputPassword.erase(inputPassword.size()-1);
                            }
                            displayedPassword.erase(displayedPassword.size()-1);
                            }
                    }
                    else if (event.text.unicode == '\r' || event.text.unicode == '\n') // Enter
                    {
                        // std::cout << "Password: " << passwordText.getString().toAnsiString() << std::endl;
                    }
                    else
                    {       displayedPassword+='*';
                            inputPassword+=static_cast<char>(event.text.unicode);
                            passwordText.setString(displayedPassword);
                    }
                }
        }

            if (isEnteringConfirmPassword && event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128)
                {
                    if (event.text.unicode == '\b') // Backspace
                    {
                        if (!confirmPasswordText.getString().isEmpty())
                            {
                                confirmPasswordText.setString(confirmPasswordText.getString().substring(0, confirmPasswordText.getString().getSize() - 1));
                                if(!inputConfirmPassword.empty())
                                {
                                    inputConfirmPassword.erase(inputConfirmPassword.size()-1);
                                }
                                displayedConfirmPassword.erase(displayedConfirmPassword.size()-1);
                            }
                    }
                    else if (event.text.unicode == '\r' || event.text.unicode == '\n') // Enter
                    {
                        // std::cout << "Confirm Password: " << confirmPasswordText.getString().toAnsiString() << std::endl;
                    }
                    else
                    {   //displaying * inpace f password characters
                        displayedConfirmPassword+='*';
                        inputConfirmPassword+=static_cast<char>(event.text.unicode);
                        confirmPasswordText.setString(displayedConfirmPassword);
                    }
                }
            }
    
    
    
    }

}


void drawUI() {

            if(UIManager::startPage){
                initUIElements();
            }
        
            float colorStepR = static_cast<float>(bottomColor.r - topColor.r) / window.getSize().y;
            float colorStepG = static_cast<float>(bottomColor.g - topColor.g) / window.getSize().y;
            float colorStepB = static_cast<float>(bottomColor.b - topColor.b) / window.getSize().y;
          for (unsigned int i = 0; i < window.getSize().y; ++i) {
            // Calculate the color at this vertical position
            sf::Color color(
                static_cast<sf::Uint8>(topColor.r + colorStepR * i),
                static_cast<sf::Uint8>(topColor.g + colorStepG * i),
                static_cast<sf::Uint8>(topColor.b + colorStepB * i)
            );

            // Create a rectangle for this row and fill it with the calculated color
            sf::RectangleShape rect(sf::Vector2f(window.getSize().x, 1));
            rect.setPosition(0, i);
            rect.setFillColor(color);
            window.draw(rect);
        }
        
        window.draw(usernameBox);
        window.draw(usernameButton);
        window.draw(usernameButtonText);
        window.draw(usernameText);
        window.draw(passwordButton);
        window.draw(passwordText);
        window.draw(passwordButtonText);
        window.draw(eyeButton);
        window.draw(confirmPasswordButton);
        window.draw(confirmPasswordButtonText);
        window.draw(confirmPasswordText);
        window.draw(confirmEyeButton);
        window.draw(SignupButton);
        window.draw(SignupButtonText);
        window.draw(toLoginButton);
        window.draw(toLoginButtonText);
        window.draw(logoSprite);
        window.draw(welcomeText);
        UIManager::startPage=false;
        
    }



    void resetUI(){
        // window.clear();
            float colorStepR = static_cast<float>(bottomColor.r - topColor.r) / window.getSize().y;
            float colorStepG = static_cast<float>(bottomColor.g - topColor.g) / window.getSize().y;
            float colorStepB = static_cast<float>(bottomColor.b - topColor.b) / window.getSize().y;
            for (unsigned int i = 0; i < window.getSize().y; ++i) {
            // Calculate the color at this vertical position
            sf::Color color(
                static_cast<sf::Uint8>(topColor.r + colorStepR * i),
                static_cast<sf::Uint8>(topColor.g + colorStepG * i),
                static_cast<sf::Uint8>(topColor.b + colorStepB * i)
            );

            // Create a rectangle for this row and fill it with the calculated color
            sf::RectangleShape rect(sf::Vector2f(window.getSize().x, 1));
            rect.setPosition(0, i);
            rect.setFillColor(color);
            window.draw(rect);
        }
        if(UIManager::checkResetUI){
            nextLoginUIElements();
        }
        window.draw(usernameBox);
        window.draw(usernameButton);
        window.draw(usernameButtonText);
        window.draw(usernameText);
        window.draw(passwordButton);
        window.draw(passwordText);
        window.draw(passwordButtonText);
        window.draw(eyeButton);
        window.draw(loginButton);
        window.draw(loginButtonText);
        window.draw(welcomeText);
        window.draw(logoSprite);
        UIManager::checkResetUI=false;
    }

     void resetMainPage(std::map<std::string, std::vector<std::string>> &genreImagePathMap){
     
        if(UIManager::checkResetMainPage){
            resetMainPageElements(genreImagePathMap);
        }

         window.draw(leftRect);
        window.draw(rightRect);
        window.draw(logoSprite);
        for (const auto& text : leftTexts) {
         window.draw(text);
        }
        // Check if sprites vector is not empty before drawing
            for (const auto& sprite : sprites) {
                window.draw(sprite);
            }
            UIManager::checkResetMainPage=false;
    }
    
    void playSongPage(std::map<std::string, std::vector<std::string>> &genreImagePathMap,std::string &clickedSongPage)
    {
        
        if(UIManager::checkPlaySongPage){
            playSongPageElements(genreImagePathMap,clickedSongPage);
        }
         
        window.draw(leftRect);
        window.draw(rightRect);
        window.draw(logoSprite);
        window.draw(quitButton);
        window.draw(playButton);
        window.draw(pauseButton);
        window.draw(resumeButton);
        window.draw(forwardButton);
        window.draw(backwardButton);
        for (const auto& text : leftTexts) {
         window.draw(text);
        }
        // Check if sprites vector is not empty before drawing
            for (const auto& sprite : sprites) {
                window.draw(sprite);
            }
            UIManager::checkPlaySongPage=false;
    }

    sf::Color hexToColor(const std::string& hexcode) {
        std::string hex = hexcode;
        if (hex[0] == '#')
            hex = hex.erase(0, 1);

        unsigned char r = (unsigned char)HexadecimalToDecimal(hex.substr(0, 2));
        unsigned char g = (unsigned char)HexadecimalToDecimal(hex.substr(2, 2));
        unsigned char b = (unsigned char)HexadecimalToDecimal(hex.substr(4, 2));
        int alphaDecimal = hex.length() == 8 ? HexadecimalToDecimal(hex.substr(6, 2)) : 255; // Default alpha to 255 if not provided
        float alpha = (float)alphaDecimal / 255.0f;

        return sf::Color(r, g, b, (sf::Uint8)(255 * alpha));
    }

    bool isValidEmailAddress(const std::string& email) {
        size_t atPos = email.find('@');
        size_t dotPos = email.rfind('.');
        return atPos != std::string::npos && dotPos != std::string::npos && dotPos > atPos;
    }
};
bool UIManager::resetui = false;
bool UIManager::nextMainPage=false;
bool UIManager::isEnteringUsername=false;
bool UIManager::isEnteringPassword=false;
bool UIManager::checkResetUI=false;
bool UIManager::checkMainPageUI=false;
std::string UIManager::selectedGenere="Bollywood";
bool UIManager::checkResetMainPage=true;
bool UIManager::genereSelectedPage= false;
bool UIManager::isEnteringConfirmPassword=false;
bool UIManager::playedSong=false;
bool UIManager::clickSongButton=false;
bool UIManager::checkPlaySongPage=true;
bool UIManager::pausedSong=false;
bool UIManager::resumeSong=false;
bool UIManager::forwardSong=false;
bool UIManager::backwardSong=false;
int  UIManager::clickedIndex=0;
bool UIManager::startPage=true;
bool UIManager::showPassword=false;
bool UIManager::quitWindow=false;




void HandleUI(ClientSessionData &clientSessionData,int &clientSocket,sql::Connection *con, sql::PreparedStatement *pstmt,std::atomic<bool> &clientRunning)
{

	std::string requestSongPlay;
    std::string clickedSongPage;
    std::map<std::string, std::vector<std::string>> genreImagePathMap;
    const std::string imagesDir = "../media/images/";

    genreImagePathMap = {
            {"Bollywood", {imagesDir + "Bollywood/track1.jpeg",
            imagesDir + "Bollywood/track2.jpeg",
            imagesDir + "Bollywood/track3.jpeg",
            imagesDir + "Bollywood/track4.jpeg",
            imagesDir + "Bollywood/track5.jpeg",
            imagesDir + "Bollywood/track6.jpeg",
            imagesDir + "Bollywood/track7.jpeg",
            imagesDir + "Bollywood/track8.jpeg",
            imagesDir + "Bollywood/track9.jpeg",
            imagesDir + "Bollywood/track10.jpeg",
            imagesDir + "Bollywood/track11.jpeg",
            imagesDir + "Bollywood/track12.jpeg"}},
            
            {"Disco", {imagesDir + "Disco/track1.jpeg",
            imagesDir + "Disco/track2.jpeg",
            imagesDir + "Disco/track3.jpeg",
            imagesDir + "Disco/track4.jpeg",
            imagesDir + "Disco/track5.jpeg",
            imagesDir + "Disco/track6.jpeg",    
            imagesDir + "Disco/track7.jpeg",
            imagesDir + "Disco/track8.jpeg",
            imagesDir + "Disco/track9.jpeg",
            imagesDir + "Disco/track10.jpeg",
            imagesDir + "Disco/track11.jpeg",}},
            
            {"Hip_Hop", {
            imagesDir + "Hip_Hop/track1.jpeg",
            imagesDir +"Hip_Hop/track2.jpeg",
            imagesDir +"Hip_Hop/track3.jpeg",
            imagesDir +"Hip_Hop/track4.jpeg",
            imagesDir +"Hip_Hop/track5.jpeg",
            imagesDir +"Hip_Hop/track6.jpeg",
            imagesDir +"Hip_Hop/track7.jpeg",
            imagesDir +"Hip_Hop/track8.jpeg",
            imagesDir +"Hip_Hop/track9.jpeg",
            imagesDir +"Hip_Hop/track10.jpeg",
            imagesDir +"Hip_Hop/track11.jpeg",
        }
        },
            {"Punjabi", {
            imagesDir +"Punjabi/track1.jpeg",
            imagesDir +"Punjabi/track2.jpeg",
            imagesDir +"Punjabi/track3.jpeg",
            imagesDir +"Punjabi/track4.jpeg",
            imagesDir +"Punjabi/track5.jpeg",
            imagesDir +"Punjabi/track6.jpeg",
            imagesDir +"Punjabi/track7.jpeg",
            imagesDir +"Punjabi/track8.jpeg",
            imagesDir +"Punjabi/track9.jpeg",
            imagesDir +"Punjabi/track10.jpeg",
            imagesDir +"Punjabi/track11.jpeg",
        }
        },
            {"Rap", {
            imagesDir +"Rap/track1.jpeg",
            imagesDir +"Rap/track2.jpeg",
            imagesDir +"Rap/track3.jpeg",
            imagesDir +"Rap/track4.jpeg",
            imagesDir +"Rap/track5.jpeg",
            imagesDir +"Rap/track6.jpeg",
            imagesDir +"Rap/track7.jpeg",
            imagesDir +"Rap/track8.jpeg",
            imagesDir +"Rap/track9.jpeg",
            imagesDir +"Rap/track10.jpeg",
            imagesDir +"Rap/track11.jpeg",
    }
}
       };

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "User Authentication");

    
    UIManager uiManager(window,genreImagePathMap);
              
    while (window.isOpen()) {
    uiManager.handleEvents(con, pstmt,genreImagePathMap,clientSessionData,clientRunning,clientSocket,requestSongPlay,clickedSongPage);
    sf::Event event;
     while (window.pollEvent(event)) {
        // Handle events
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
    if ( UIManager::resetui ) {
        // Clear the window and reset UI
        window.clear();
        uiManager.resetUI();
        
    } 
    else 
        if (UIManager::genereSelectedPage){
         window.clear();
         uiManager.resetMainPage(genreImagePathMap);
    }
        else 
            if(UIManager::clickSongButton)
            {
                    
                    window.clear();
                    uiManager.playSongPage(genreImagePathMap,clickedSongPage);

                    if(UIManager::playedSong){
                        
                        if(clientSessionData.m_songStarted)
                        {
                            clientSessionData.m_streamingMusic.StopMusic();
                            clientSessionData.m_songStarted = false;
                        }
                        
                        Packet requestSong;
                        clientSessionData.m_streamingMusic.SetSongName(requestSongPlay.c_str());		
                        CreateRequestSongPacket(requestSong, requestSongPlay);
                        PutPacketOnSendQueue(requestSong, clientSessionData.m_networkTraffic);
                        UIManager::playedSong=false;

            }
            else 
                if(UIManager::pausedSong){
                clientSessionData.m_streamingMusic.PauseMusic();
                UIManager::pausedSong=false;
                }
                else 
                    if(UIManager::resumeSong){
                        clientSessionData.m_streamingMusic.ResumeMusic();
                        UIManager::resumeSong=false;
                    }
                    else 
                        if(UIManager::forwardSong){
                            clientSessionData.m_streamingMusic.ForwardMusic();
                            UIManager::forwardSong=false;
                        }
                        else if(UIManager::backwardSong){
                            clientSessionData.m_streamingMusic.BackwardMusic();
                            UIManager::backwardSong=false;
                            }
                            else 
                                if(UIManager::quitWindow){
                                    if(clientSessionData.m_songStarted){
                                            clientSessionData.m_streamingMusic.StopMusic();
                                            clientSessionData.m_songStarted = false;
                                    }
                                    return ;
                                    }

            }

    else
    {   
        window.clear();
        uiManager.drawUI();
    } 
    window.display();
    }
    
    
    };

}