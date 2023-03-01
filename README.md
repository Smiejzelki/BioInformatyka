# Praca na konkurs Motorola Science Cup
## Budowanie projektu
### Windows
#### Należy uruchomić skrypt GenerateVS2022.bat znajdujący się w folderze ProjectScripts/VisualStudio. Wymagane jest środowisko Visual Studio 2022.
`git clone https://github.com/Smiejzelki/BioInformatyka.git`
`cd BioInformatyka/ProjectScripts/VisualStudio`
`GenerateVS2022.bat`
## Dokumentacja
### Dokumentacje projektu można znaleść w folderze Projekt/Dokumentacja pod nazwą instrukcja.html.

##  Struktura plików
    Bioinformatyka
    ├── BioInformatyka                  # Kod biblioteki "BioInformatyka"
    |   ├── include (...)               # Pliki nagłówkowe
    |   ├── src                         # Pliki źródłowe
    |   |   ├── Application.cpp         # Odpowiada za inicjalizacje aplikacji 
    |   |   ├── Camera.cpp              # Implementacja prostokątnej kamery
    |   |   ├── Canvas.cpp              # Implementacja płótna
    |   |   ├── Event.cpp               # Wydarzenia
    |   |   ├── GUI.cpp                 # Funkcje graficznego interface'u
    |   |   ├── GUIRenderer.cpp         # Zarządza renderowaniem interface'u
    |   |   └── Renderer.cpp            # Tekstury, shadery, pamięć grafiki, ramka okna
    |   ├── platform                    # Kod zależny od platformy
    |   |   └── Windows                 # Kod platformy windows
    |   |       ├── WindowsWindow.cpp   # Okno, wejście/wyjście             
    |   |       └── WindowsPlatform.cpp # Konsola, schowek, zarządzanie plikami 
    |   ├── thirdparty (...)            # Użyte biblioteki
    |   └── (...)
    ├── ProjectScripts (...)            # Skrypty do budowy projektu
    ├── Projekt (...)                   # Skompilowany program
    ├── Testy (...)                     # Testy jednostkowe
    ├── Wizualizator                    # Kod aplikacji wizualizatora
    |   ├── include                     # Pliki nagłówkowe
    |   |   ├── framework               # Biblioteka nagłówkowa abstrakcji sekwencji
    |   |   |   ├── AlphabetBase.hpp    # Stanowi podstawe sekwencji
    |   |   |   ├── Dna.hpp             # Nukleotyd (gwarancja poprawności)
    |   |   |   ├── DnaX.hpp            # Nukleotyd (możliwy stan niewłaściwy)
    |   |   |   ├── Rna.hpp             # Rna (gwarancja poprawności)
    |   |   |   ├── RnaX.hpp            # Rna (możliwy stan niewłaściwy)
    |   |   |   ├── Amino.hpp           # Aminokwas (gwarancja poprawności)
    |   |   |   ├── AminoX.hpp          # Aminokwas (możliwy stan niewłaściwy)
    |   |   |   ├── Elements.hpp        # Węgiel, azot, tlen, siarka, wodór
    |   |   |   ├── Hydropathy.hpp      # Hydrofobowość
    |   |   |   ├── Nucleotides.hpp     # Konwersja sekwencji
    |   |   |   ├── Transform.hpp       # Obliczanie własności
    |   |   |   └── (...)
    |   |   └── (...)
    |   ├── src                         # Pliki źródłowe
    |   |   ├── FastaReader.cpp         # Parser formatu fasta
    |   |   ├── Projet.cpp              # Abstrakcja projektu
    |   |   ├── Wizualizator.cpp        # Wizualizator, zarządzanie projektem
    |   |   └── Panels
    |   |       ├── PanelBase.cpp       # Interface panelu
    |   |       ├── ContentPanel.cpp    # Panel sekwencji
    |   |       ├── PlotPanel.cpp       # Panel otwartych ramek odczytu
    |   |       ├── ProjectPanel.cpp    # Drzewo plików projektu, wgląd w sekwencje
    |   |       ├── PropertiesPanel.cpp # Panel z właściwościami chem., fiz. i biol.
    |   |       └── StructurePanel.cpp  # Dwuwymiarowa wizualizacja struktury peptydów
    |   └── (...)    
    └── (...)
