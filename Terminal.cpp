// ============================================================================
// Terminal Kernel (Implementation)
// Programmed by Francois Lamini
// ============================================================================
#include "Terminal.h"

Codeloader::cAllegro_IO* allegro = NULL;
Codeloader::cCommand_Processor* command = NULL;

bool Process_Command();
bool Process_Keys();

// **************************************************************************
// Program Entry Point
// **************************************************************************

int main(int argc, char** argv) {
  try {
    Codeloader::cConfig config("Config");
    int width = config.Get_Property("width");
    int height = config.Get_Property("height");
    allegro = new Codeloader::cAllegro_IO("Codeloader Terminal", width, height, 2, "Source_Code");
    int letter_w = config.Get_Property("letter-w");
    int letter_h = config.Get_Property("letter-h");
    command = new Codeloader::cCommand_Processor(width / (letter_w / 2), height / letter_h, letter_w / 2, letter_h, allegro); // Give space for cursor.
    allegro->Process_Messages(Process_Command, Process_Keys);
  }
  catch (Codeloader::cError error) {
    error.Print();
  }
  if (allegro) {
    delete allegro;
  }
  if (command) {
    delete command;
  }
  std::cout << "Done." << std::endl;
  return 0;
}

// **************************************************************************
// Command Processor
// **************************************************************************

/**
 * Called when command needs to be processed.
 * @return True if the app needs to exit, false otherwise.
 */
bool Process_Command() {
  bool done = false;
  if (command->status == Codeloader::eSTATUS_DONE) {
    done = true;
  }
  else {
    command->Read_Input();
    command->Render();
  }
  return done;
}

/**
 * Called when keys are processed.
 * @return True if the app needs to exit, false otherwise.
 */
bool Process_Keys() {
  command->Process_Keys();
  return false;
}

namespace Codeloader {

  // **************************************************************************
  // Command Processor Implementation
  // **************************************************************************

  /**
   * Initializes the command processor.
   * @param width The width of the terminal.
   * @param height The height of the terminal.
   * @param letter_w The width of a letter.
   * @param letter_h The height of a letter.
   * @parma io The I/O control.
   */
  cCommand_Processor::cCommand_Processor(int width, int height, int letter_w, int letter_h, cIO_Control* io) : cTerminal(width, height, letter_w, letter_h, io) {
    this->root = ".";
  }

  /**
   * Called when a readline is processed.
   * @param text The result text.
   */
  void cCommand_Processor::On_Read(std::string text) {
    cArray<std::string> params = Parse_C_Lesh_Line(text);
    int param_count = params.Count();
    for (int param_index = 0; param_index < param_count; param_index++) {
      std::string param = params[param_index];
      this->command.Push(param);
    }
    this->Execute_Command();
  }

  /**
   * Executes a single command.
   */
  void cCommand_Processor::Execute_Command() {
    if (this->command.Count() > 0) { // Only execute command if there is anything in the buffer.
      std::string code = this->Get_Param();
      if (code == "print") {
        std::string text = Codeloader::C_Lesh_String_To_Cpp_String(this->Get_Param());
        this->Write_String(text + '\n');
      }
      else if (code == "list") {
        std::string path = al_get_current_directory();
        cAllegro_IO* allegro = static_cast<cAllegro_IO*>(this->io);
        cArray<std::string> files = allegro->Get_File_List(path);
        int file_count = files.Count();
        for (int file_index = 0; file_index < file_count; file_index++) {
          this->Write_String(files[file_index] + '\n');
        }
      }
      else if (code == "cd") {
        std::string path = al_get_current_directory();
        std::string folder = path + ALLEGRO_NATIVE_PATH_SEP + Codeloader::C_Lesh_String_To_Cpp_String(this->Get_Param());
        bool result = al_change_directory(folder.c_str());
        if (!result) {
          this->Set_Error_Mode(true);
          this->Write_String("Folder does not exist.\n");
        }
      }
      else if (code == "version") {
        this->Write_String("Codeloader Terminal v1\n");
      }
      else if (code == "make") {
        std::string path = al_get_current_directory();
        std::string folder = path + ALLEGRO_NATIVE_PATH_SEP + Codeloader::C_Lesh_String_To_Cpp_String(this->Get_Param());
        bool result = al_make_directory(folder.c_str());
        if (!result) {
          this->Set_Error_Mode(true);
          this->Write_String("Could not create folder.\n");
        }
      }
      else if (code == "exit") {
        this->status = Codeloader::eSTATUS_DONE;
      }
      else {
        // Clear command.
        while (this->command.Count() > 0) {
          this->command.Pop();
        }
        this->Set_Error_Mode(true);
        this->Write_String("Invalid command " + code + ".\n");
      }
    }
  }

  /**
   * Gets a single parameter.
   * @return The parameter or an empty string if there is no parameter.
   */
  std::string cCommand_Processor::Get_Param() {
    std::string param = "";
    if (this->command.Count() > 0) {
      param = this->command.Shift();
    }
    return param;
  }

}