//codigo reaproveitado de https://www.youtube.com/watch?v=T31MoLJws4U&t=1788s

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

namespace Estados
{
    namespace Menus
    {
        class Textbox
        {
        private:
            bool isSelected = false;
            sf::Text textbox; 
            std::ostringstream text;
            bool hasLimit = false;
            int limit;


            void inputLogic(int charTyped);
            void deleteLastChar();
        public:
            Textbox(){}
            Textbox(int size, sf::Color color, bool sel);
            ~Textbox() {}
            void setFont(sf::Font &font) {textbox.setFont(font);}
            void setPosition(sf::Vector2f pos){textbox.setPosition(pos);}
            void setLimit(bool ToF, int lim); 
            void setLimit(bool ToF) {hasLimit = ToF;}
            void setSelected(bool sel) ;
            std::string getText() {return text.str();} 
            void drawTo(sf::RenderWindow &window){window.draw(textbox);}   
            void typedOn(sf::Event input);
        
        };  
    }
}