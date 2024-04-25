// ============================================================================
// Codeloader Terminal (Definitions)
// Programmed by Francois Lamini
// ============================================================================

#include "..\Code_Helper\Codeloader.hpp"
#include "..\Code_Helper\Allegro.hpp"

namespace Codeloader {

  class cCommand_Processor : public Codeloader::cTerminal {

    public:
      cArray<std::string> command;
      std::string root;

      cCommand_Processor(int width, int height, int letter_w, int letter_h, Codeloader::cIO_Control* io);
      void On_Read(std::string text);
      void Execute_Command();
      std::string Get_Param();

  };

}
