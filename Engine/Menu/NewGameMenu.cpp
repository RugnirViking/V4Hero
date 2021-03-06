#include "NewGameMenu.h"
#include "../V4Core.h"
#include "ButtonList.h"
#include "iostream"
#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
NewGameMenu::NewGameMenu()
{
    //ctor
    f_font.loadFromFile("resources/fonts/patapon.ttf");
    //f_font.loadFromFile("resources/fonts/arial.ttf");
    t_title.setFont(f_font);

    t_title.setCharacterSize(112);
    //t_title.setColor(sf::Color::White);
    t_title.setFillColor(sf::Color::White);
    t_title.setString("PATAFOUR - NEW GAME");
    t_title.setOrigin(t_title.getGlobalBounds().width / 2, t_title.getGlobalBounds().height / 2);

    //t_pressToContinue.setFont(f_font);
    //t_pressToContinue.setCharacterSize(32);
    //t_pressToContinue.setColor(sf::Color::White);
    //t_pressToContinue.setFillColor(sf::Color::White);
    //t_pressToContinue.setString("Press any key to continue...");
    //t_pressToContinue.setOrigin(t_pressToContinue.getGlobalBounds().width/2,t_pressToContinue.getGlobalBounds().height/2);

    mm_bigBox.setSize(sf::Vector2f(100, 10));
    mm_bigBox.setFillColor(sf::Color(4, 0, 90));

    mm_titleBox.setSize(sf::Vector2f(100, 10));
    mm_titleBox.setFillColor(sf::Color::Red);
    is_active = true;

    // this is outside the loop
    startAlpha = 255;
    endAlpha = 0;
    targetTime = sf::seconds(10);


    fade.setPosition(sf::Vector2f(0, 0));
    fade.setFillColor(sf::Color(0, 0, 0, 0));
    fade.setSize(sf::Vector2f(800, 600));
}
void NewGameMenu::Initialise(Config* thisConfigs, V4Core* parent)
{
    Scene::Initialise(thisConfigs, parent);
    optionsMenu.Initialise(thisConfigs, parent, this);
    newGameNameEntryMenu.Initialise(thisConfigs, parent, this);
    v4Core->menus.push_back(&optionsMenu);
    buttonList.Initialise(&f_font, *thisConfig, &(v4Core->currentController), this, &optionsMenu, &newGameNameEntryMenu);
}
void NewGameMenu::EventFired(sf::Event event)
{
    if (v4Core->currentController.isInitialized)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            // do something here;
            buttonList.KeyPressedEvent(event);
            if (event.key.code == sf::Keyboard::Escape)
            {
                spdlog::debug("Returning to patapolis menu...");
                v4Core->currentController.StopMission();
                v4Core->mainMenu.Show();
                v4Core->mainMenu.patapolisMenu.Hide();
            }
        }
    } else if (is_active)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            // do something here;
            buttonList.KeyPressedEvent(event);
            if (event.key.code == sf::Keyboard::Escape)
            {
                spdlog::debug("Returning to main menu...");
                v4Core->currentController.StopMission();
                is_active = true;
            }
        } else if (event.type == sf::Event::MouseButtonReleased)
        {
            // We use mouse released so a user can change their mind by keeping the mouse held and moving away.
            buttonList.MouseReleasedEvent(event);
        }
    } else if (optionsMenu.is_active)
    {
        optionsMenu.EventFired(event);
    } else if (newGameNameEntryMenu.is_active)
    {
        newGameNameEntryMenu.EventFired(event);
    } else if (newGameNameEntryMenu.savefilecreated.is_active)
    {
        newGameNameEntryMenu.savefilecreated.EventFired(event);
    }
}
void NewGameMenu::Update(sf::RenderWindow& window, float fps, InputController& inputCtrl, InputController& inputCtrlHeld)
{

    if (is_active)
    {
        // this is inside the loop
        sf::Time currentTime = timer.getElapsedTime();
        int currentAlpha = endAlpha;
        if (currentTime >= targetTime)
        {
            // oops: currentAlpha = endAlpha; // make certain that the alpha is at its final destination
            //you are done
            currentAlpha = endAlpha;
        } else
        {
            currentAlpha = startAlpha + (endAlpha - startAlpha) * (currentTime.asMilliseconds() / (targetTime.asMilliseconds() + 0.0));
        }
        // apply alpha to whatever colour is previously set
        sf::Color fadeColor = fade.getFillColor();
        fadeColor.a = currentAlpha;
        fade.setFillColor(fadeColor);
        mm_bigBox.setSize(sf::Vector2f(window.getSize().x, window.getSize().y - 200));
        fade.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));

        fade.setPosition(0, 0);
        //mm_smallerBox.setSize(sf::Vector2f(100,10));
        //mm_titleBox.setSize(sf::Vector2f(100,10));

        mm_bigBox.setPosition(0, 85);
        //mm_smallerBox.setPosition(100,10);
        //mm_titleBox.setPosition(100,10);


        window.draw(mm_bigBox);
        //window.draw(mm_smallerBox);
        //window.draw(mm_titleBox);

        t_title.setPosition(window.getSize().x / 2, 200);
        window.draw(t_title);


        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        auto lastView = window.getView();
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, lastView);

        //t_pressToContinue.setPosition(window.getSize().x/2,300);
        //window.draw(t_pressToContinue);

        buttonList.Update(window, fps, &worldPos);
        window.draw(fade);
        window.setView(window.getDefaultView());

    } else
    {
        if (v4Core->currentController.isInitialized && v4Core->currentController.isFinishedLoading)
        {
            v4Core->currentController.Update(window, fps, inputCtrl);
        } else if (optionsMenu.is_active)
        {
            optionsMenu.Update(window, fps, inputCtrl);
        } else if (newGameNameEntryMenu.is_active)
        {
            newGameNameEntryMenu.Update(window, fps);
        } else if (newGameNameEntryMenu.savefilecreated.is_active)
        {
            newGameNameEntryMenu.savefilecreated.Update(window, fps);
        }
    }
}
void NewGameMenu::UpdateButtons()
{
    /// this should update the text on all the buttons
    buttonList.UpdateButtons();
}
void NewGameMenu::OnExit()
{
    /// when we exit the main menu, we do nothing for now.
    /// perhaps we would want to unload sprites or songs etc
}
NewGameMenu::~NewGameMenu()
{
    //dtor
}
