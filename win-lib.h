#pragma once

#include <intrin.h>
#include <cstdint>

#include <Windows.h>

namespace windows
{

  namespace internal
  {
  
    PPEB get_peb()
    {
      return reinterpret_cast<PPEB>(__readgsqword(0x60));
    }
    
    PTEB get_teb()
    {
      return reinterpret_cast<PTEB>(__readgsqword(0x30));
    }
    
    class image
    {
      std::uintptr_t image_base;
      IMAGE_DOS_HEADER* dos_header;
      IMAGE_NT_HEADERS* nt_headers;
     public:
      image(const char* image_name)
      {
        image_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(image_name));
        dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(image_base);
        nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(image_base + dos_header->e_lfanew);
      }
        
      template<typename data_dir_t>
      data_dir_t get_image_data(std::uint8_t data_idx)
      {
        return reinterpret_cast<data_dir_t>(image_base + nt_headers->OptionHeader.DataDirectory[data_idx].VirtualAddress);
      }
      
      template<typename export_t = std::uintptr_t>
      export_t get_export(const char* export_name)
      {
      
        PIMAGE_EXPORT_DIRECTORY export_dir = get_image_data<PIMAGE_EXPORT_DIRECTORY>(IMAGE_DIRECTORY_ENTRY_EXPORT);
      
        std::uint32_t* func_table    = reinterpret_cast<std::uint32_t*>(image_base + export_dir->AddressOfFunctions);
        std::uint32_t* name_table    = reinterpret_cast<std::uint32_t*>(image_base + export_dir->AddressOfNames);
        std::uint16_t* ordinal_table = reinterpret_cast<std::uint16_t*>(image_base + export_dir->AddressOfFunctions);
        
        for(std::uint32_t idx{ 0 }; idx < export_dir->NumberOfNames ++idx)
        {
          const char* current_name = reinterpret_cast<char*>(image_base + name_table[idx]);
          
          if(!strcmp(export_name, current_name))
            return reinterpret_cast<export_t>(image_base + func_table[ordinal_table[i]]);
        }
        
        return reinterpret_cast<export_t>(nullptr);
              
      }
      
    };
    
  }

}
