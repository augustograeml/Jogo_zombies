#include "../Estados/Menus/textbox.h"
#define DELETE_TECLA 8
#define ENTER_TECLA 13
#define ESC_TECLA 27

namespace Estados
{
    namespace Menus
    {
        Textbox::Textbox(int size, sf::Color color, bool sel)
        {
            textbox.setCharacterSize(size);
            textbox.setColor(color);
            isSelected = sel;
            if (sel)
            {
                textbox.setString("_");
            }
            else
            {
                textbox.setString("");
            }
        }
        void Textbox::inputLogic(int charTyped)
        {
            if (charTyped != DELETE_TECLA && charTyped != ENTER_TECLA && charTyped != ESC_TECLA)
            {
                text << static_cast<char>(charTyped);
            }
            else if (charTyped == DELETE_TECLA)
            {
                if (text.str().length() > 0)
                {
                    deleteLastChar();
                }
            }
            textbox.setString(text.str() + "_");
        }
        void Textbox::deleteLastChar()
        {
            std::string t = text.str();
            std::string newT = "";
            for (int i = 0; i < t.length(); i++)
            {
                newT += t[i];
            }
            text.str("");
            text << newT;

            textbox.setString(text.str());
        }
        void Textbox::setLimit(bool ToF, int lim)
        {
            hasLimit = ToF;
            limit = lim;
        }
        void Textbox::setSelected(bool sel)
        {
            isSelected = sel;
            if (!sel)
            {
                std::string t = text.str();
                std::string newT = "";
                for (int i = 0; i < t.length(); i++)
                {
                    newT += t[i];
                }
                textbox.setString(newT);
            }
        }
        void Textbox::typedOn(sf::Event input)
        {
            if (isSelected)
            {
                int charTyped = input.text.unicode;
                if (charTyped < 128)
                {
                    if (hasLimit)
                    {
                        if (text.str().length() <= limit)
                        {
                            inputLogic(charTyped);
                        }
                        else if (text.str().length() > limit && charTyped == DELETE_TECLA)
                        {
                            deleteLastChar();
                        }
                    }
                    else
                    {
                        inputLogic(charTyped);
                    }
                }
            }
        }
    }
}
