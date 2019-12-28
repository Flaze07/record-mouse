#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <atomic>
#include <thread>
#include <sstream>
#include <iterator>

class Manager
{
private:
    POINT p;
    std::vector<std::string> action;
    std::atomic<bool> stop;
public:
    void run();
    void record();
    void play();
};

std::vector<std::string> split(std::string const &input )
{
    std::istringstream buffer( input );
    std::vector<std::string> ret{ std::istream_iterator<std::string>( buffer),
                                  std::istream_iterator<std::string>() };
    return ret;
}

void LeftClick()
{
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput( 1, &Input, sizeof( INPUT ) );

    ::ZeroMemory( &Input, sizeof( INPUT ) );
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    ::SendInput( 1, &Input, sizeof( INPUT ) );
}

void RightClick()
{
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    ::SendInput( 1, &Input, sizeof( INPUT ) );

    ::ZeroMemory( &Input, sizeof( INPUT ) );
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    ::SendInput( 1, &Input, sizeof( INPUT ) );
}

int main()
{
    Manager manager;
    manager.run();
}

void Manager::run()
{
    std::string input;
    do
    {
        std::cin >> input;
        if( input == "record" ) record();
        else if( input == "play" )
        {
            stop.store( false );
            std::thread playThread( play, this );
            std::cin.get();
            stop.store( true );
            playThread.join();
        }
    }while( input != "quit" );
}

void Manager::record()
{
    std::string input;
    action.clear();
    do
    {
        std::cin >> input;
        if( input == "pos" )
        {
            ::GetCursorPos( &p );
            action.emplace_back( std::to_string( p.x ) + " " + std::to_string( p.y ) );
        }
        else if( input == "left" )
        {
            action.emplace_back( "left" );
        }
        else if( input == "right" )
        {
            action.emplace_back( "right" );
        }
    }while( input != "stop" );
}

void Manager::play()
{
    int index = 0;
    while( stop.load() == false )
    {
        if( action.at( index ) == "left" )
        {
            LeftClick();
        }
        else if( action.at( index ) == "right" )
        {
            RightClick();
        }
        else
        {
            std::vector<std::string> coords = split( action.at( index ) );
            int x = std::stoi( coords.at( 0 ) );
            int y = std::stoi( coords.at( 1 ) );
            ::SetCursorPos( x, y );
        }

        if( ++index == action.size() ) index = 0;
        std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
    }
}
