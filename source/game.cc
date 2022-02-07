#include "../include/game.h"

Game::Game() {
    window = nullptr;
    renderer = nullptr;
    for(int i = 0; i < NUM_OF_PIECES; i++) {
        pieces[i] = nullptr;
    }
    screenWidth = 600;
    screenHeight = 600;
    squareEdge = 0;
    boardStartingX = 0;
    boardStartingY = 0;
    running = false;
    dragPieceByte = 0b00000000;
    dragPieceTextureMouseX = 0;
    dragPieceTextureMouseY = 0;
    state = new State;
}

Game::~Game() {
    state->~State();
    state = nullptr;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    for(int i = 0; i < 12; i++) {
        SDL_DestroyTexture(pieces[i]);
    }
    IMG_Quit();
};

bool Game::isRunning() {
    return running;
}

void Game::init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Failed to initiate SDL: " << SDL_GetError() << std::endl;
        return;
    }
    else {
        std::cout << "Succesfully initiated SDL" << std::endl;
    }

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
       std::cerr << "Failed to initiate IMG: " << SDL_GetError() << std::endl;
       return;
    }
    else {
        std::cout << "Succesfully initiated IMG" << std::endl;
    }

    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                     screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    renderer = SDL_CreateRenderer(window, -1, 0);

    Game::loadPieces();

    Game::renderBoard();
    
    running = true;
}

void Game::eventHandler() {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch(event.type) {
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Game::resizeWindow(event.window.data2, event.window.data1);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT) {
                if(dragPieceByte == 0b00000000) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    Game::pickupDragPiece(mouseX, mouseY);
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(dragPieceByte != 0b00000000) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                Game::releaseDragPiece(mouseX, mouseY);
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
    
    Game::renderBoard();
    Game::renderState();

    if(dragPieceByte != 0b00000000) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Game::renderDragPiece(mouseX, mouseY);
    }

    SDL_RenderPresent(renderer);
}

void Game::loadPieces() {
    char* PNGLocations[NUM_OF_PIECES];
    PNGLocations[0] = (char*)DARK_PAWN_IMG;
    PNGLocations[1] = (char*)LIGHT_PAWN_IMG;
    PNGLocations[2] = (char*)DARK_KNIGT_IMG;
    PNGLocations[3] = (char*)LIGHT_KNIGHT_IMG;
    PNGLocations[4] = (char*)DARK_BISHOP_IMG;
    PNGLocations[5] = (char*)LIGHT_BISHOP_IMG;
    PNGLocations[6] = (char*)DARK_ROOK_IMG;
    PNGLocations[7] = (char*)LIGHT_ROOK_IMG;
    PNGLocations[8] = (char*)DARK_QUEEN_IMG;
    PNGLocations[9] = (char*)LIGHT_QUEEN_IMG;
    PNGLocations[10] = (char*)DARK_KING_IMG;
    PNGLocations[11] = (char*)LIGHT_KING_IMG;

    for(int i = 0; i < NUM_OF_PIECES; i++) {
        SDL_Surface* image = IMG_Load(PNGLocations[i]);
        if(!image) {
            std::cerr << "Failed to load image from: " << PNGLocations[i] << std::endl;
        }
        pieces[i] = SDL_CreateTextureFromSurface(renderer, image);
        if(pieces[i] == NULL) {
            std::cerr << "Failed to create texture from image: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(image);
    }
}

SDL_Texture* Game::getTexturePieceFromByte(uint8_t byte) {

    // Mapping the byte for a piece to its corresponding index in the 'pieces' array
    int texturePiecesIndex = 1 * ((byte & 0b10000000) > 0);
    switch(byte & 0b00111111) {
        case 0b00000001:
            texturePiecesIndex += 0;
            break;
        case 0b00000010:
            texturePiecesIndex += 2;
            break;
        case 0b00000100:
            texturePiecesIndex += 4;
            break;
        case 0b00001000:
            texturePiecesIndex += 6;
            break;
        case 0b00010000:
            texturePiecesIndex += 8;
            break;
        case 0b00100000:
            texturePiecesIndex += 10;
            break;
    }
    return pieces[texturePiecesIndex];
}

void Game::renderBoard() {
    squareEdge = (screenHeight * (screenHeight < screenWidth) 
                         + screenWidth * (screenHeight >= screenWidth)) / 8;

    boardStartingX = ((screenWidth - (squareEdge * 8)) / 2) * (screenWidth > squareEdge);
    boardStartingY = ((screenHeight - (squareEdge * 8)) / 2) * (screenHeight > squareEdge);

    for(int y = 0; y < 8; y++) {
        for(int x = 0; x < 8; x++) {
            SDL_Rect rect;
            rect.w = squareEdge;
            rect.h = squareEdge;
            rect.x = boardStartingX + x*squareEdge;
            rect.y = boardStartingY + y*squareEdge;
            if((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1)) {
                SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
            } // white plane
            else {
                SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
            } // black plane
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Game::renderState() {
    for(int y = 0; y < 8; y++) {
        for(int x = 0; x < 8; x++) {
            uint8_t byteFromByteBoard = state->getByteFromByteBoard(x, y);

            if(byteFromByteBoard == 0b00000000) {
                continue;
            }
            
            SDL_Rect rect;
            rect.w = squareEdge;
            rect.h = squareEdge;
            rect.x = boardStartingX + x*squareEdge;
            rect.y = boardStartingY + y*squareEdge;
            SDL_RenderCopy(renderer, Game::getTexturePieceFromByte(byteFromByteBoard), NULL, &rect);
        }
    }
}

void Game::resizeWindow(int const height, int const width) {
    screenHeight = height * (height > MIN_SCREEN_HEIGHT) 
                   + MIN_SCREEN_HEIGHT * (height <= MIN_SCREEN_HEIGHT);
    screenWidth = width * (width > MIN_SCREEN_WIDTH) 
                  + MIN_SCREEN_WIDTH * (width <= MIN_SCREEN_WIDTH);
}

void Game::pickupDragPiece(int const mouseX, int const mouseY) {
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;

    // if mouse coords are inside chess board, try to pick up a piece
    if((squareXOfMouse >= 0 && squareXOfMouse < 8) && (squareYOfMouse >= 0 && squareYOfMouse < 8)) {
        dragPieceTextureMouseX = (mouseX - boardStartingX) % squareEdge;
        dragPieceTextureMouseY = (mouseY - boardStartingY) % squareEdge;
        dragPieceByte = state->getByteFromByteBoard(squareXOfMouse, squareYOfMouse);

        if(!((dragPieceByte == 0b00000000) || (dragPieceByte & 0b01000000))) {
            state->setByteInByteBoard(squareXOfMouse, squareYOfMouse, 0b00000000);
            dragPieceInitialSquareX = squareXOfMouse;
            dragPieceInitialSquareY = squareYOfMouse;
        } // there is a piece and its of the user's side
        else {
            dragPieceByte = 0b00000000;
        }
    }
}

void Game::renderDragPiece(int const mouseX, int const mouseY) {
    SDL_Rect rect;
    rect.w = squareEdge;
    rect.h = squareEdge;
    rect.x = mouseX - dragPieceTextureMouseX;
    rect.y = mouseY - dragPieceTextureMouseY;

    // Conditions for preventing dragging outside of chess board
    if(rect.x < boardStartingX - (squareEdge / 2)) {
        rect.x = boardStartingX - (squareEdge / 2);
    }
    else if(rect.x > (boardStartingX + 8*squareEdge - (squareEdge / 2))) {
        rect.x = boardStartingX + 8*squareEdge - (squareEdge / 2);
    }
    if(rect.y < boardStartingY - (squareEdge / 2)) {
        rect.y = boardStartingY - (squareEdge / 2);
    }
    else if(rect.y > (boardStartingY + 8*squareEdge - (squareEdge / 2))) {
        rect.y = boardStartingY + 8*squareEdge - (squareEdge / 2);
    }

    SDL_RenderCopy(renderer, Game::getTexturePieceFromByte(dragPieceByte), NULL, &rect);
}

void Game::releaseDragPiece(int const mouseX, int const mouseY) {
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;

    // Checks if mouse is inside board, it's the user's turn and it's a legal move, 
    // else the piece goes back to initial square
    if(((squareXOfMouse >= 0 && squareXOfMouse < 8) && (squareYOfMouse >= 0 && squareYOfMouse < 8))
        && state->getTurn() 
        && state->isLegalMove(dragPieceByte, 
                              dragPieceInitialSquareX, dragPieceInitialSquareY, 
                              squareXOfMouse, squareYOfMouse)) {
        state->setByteInByteBoard(squareXOfMouse, squareYOfMouse, dragPieceByte);
    }
    else {
        state->setByteInByteBoard(dragPieceInitialSquareX, dragPieceInitialSquareY, dragPieceByte);
    }

    dragPieceByte = 0b00000000;
    dragPieceInitialSquareX = 0;
    dragPieceInitialSquareY = 0;
    dragPieceTextureMouseX = 0;
    dragPieceTextureMouseY = 0;
}