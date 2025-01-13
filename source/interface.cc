#include "../include/interface.h"

Interface::Interface()
{
    window = nullptr;
    renderer = nullptr;
    for (int i = 0; i < NUM_OF_PIECES; i++)
    {
        pieces[i] = nullptr;
    }
    screenWidth = STARTING_SCREEN_WIDTH;
    screenHeight = STARTING_SCREEN_HEIGHT;
    squareEdge = 0;
    boardStartingX = 0;
    boardStartingY = 0;
    running = false;
    dragPieceByte = NO_PIECE;
    dragPiecePos = -1;
    dragPieceLegalMoves = nullptr;
    game = new Game;
}

Interface::~Interface()
{
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    for (int i = 0; i < NUM_OF_PIECES; i++)
    {
        if (pieces[i])
        {
            SDL_DestroyTexture(pieces[i]);
        }
    }
    IMG_Quit();
    SDL_Quit();
    if (game)
    {
        delete game;
    }
    if (dragPieceLegalMoves)
    {
        delete dragPieceLegalMoves;
    }
};

bool Interface::isRunning()
{
    return running;
}

void Interface::menuGamemode(bool const color)
{
    while (true)
    {
        std::string inputGamemode;
        if (color)
        {
            std::cout << ">> Input the gamemode for white" << std::endl;
        }
        else
        {
            std::cout << ">> Input the gamemode for black" << std::endl;
        }
        std::cout << ">> \"1\": player | \"2\": random move computer" << std::endl
                  << std::endl;
        getline(std::cin, inputGamemode);
        std::cout << std::endl;
        if (inputGamemode.length() == 1)
        {
            switch (inputGamemode[0])
            {
            case '1':
                game->setGamemode(color, 1);
                return;
            case '2':
                game->setGamemode(color, 2);
                return;
            }
        }
        std::cerr << ">> Invalid input, please try again" << std::endl
                  << std::endl;
    }
}

void Interface::menuFEN()
{
    while (true)
    {
        std::string FEN;
        std::cout << ">> Input a FEN-notation for the game or \"default\" for the standard opening position" << std::endl
                  << ">> Example of expected input: \"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3\"" << std::endl
                  << std::endl;
        getline(std::cin, FEN);
        std::cout << std::endl;
        if (FEN == "default")
        {
            FEN = STANDARD_OPENING_FEN;
        }
        if (game->state->setStateFromFEN(FEN))
        {
            return;
        } // correct FEN notation
        std::cerr << ">> The given FEN-notation is invalid: " << FEN << std::endl
                  << ">> Please try again " << std::endl
                  << std::endl;
    }
}

bool Interface::menu()
{
    while (true)
    {
        std::string input;
        std::cout << ">> What would you like to do? (Input a letter)" << std::endl
                  << "\"P\": Play a Chess game" << std::endl
                  << "\"C\": Change the starting position of the Chess game" << std::endl
                  << "\"Q\": Quit program" << std::endl
                  << std::endl;
        getline(std::cin, input);
        std::cout << std::endl;
        if (input.length() == 1)
        {
            switch (input[0])
            {
            case 'P':
            case 'p':
                menuGamemode(WHITE);
                menuGamemode(BLACK);
                std::cout << "The game will begin. Good luck!" << std::endl
                          << std::endl;
                return true;
            case 'C':
            case 'c':
                menuFEN();
                render();
                break;
            case 'Q':
            case 'q':
                return false;
            default:
                std::cout << "Invalid input, please try again" << std::endl
                          << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid input, please try again" << std::endl
                      << std::endl;
        }
    }
}

uint8_t Interface::menuPawnPromotion()
{
    bool const colorOfDragPiece = getColorOfPiece(dragPieceByte);
    while (true)
    {
        std::string input;
        std::cout << ">> Input the first letter of the piece you would like to promote your pawn to" << std::endl
                  << ">> \"q\": queen | \"r\": rook | \"b\": bishop | \"k\": knight" << std::endl
                  << std::endl;
        getline(std::cin, input);
        if (input.length() == 1)
        {
            switch (input[0])
            {
            case 'k':
            case 'K':
                return colorOfDragPiece ? WHITE_KNIGHT : BLACK_KNIGHT;
            case 'b':
            case 'B':
                return colorOfDragPiece ? WHITE_BISHOP : BLACK_BISHOP;
            case 'r':
            case 'R':
                return colorOfDragPiece ? WHITE_ROOK : BLACK_ROOK;
            case 'q':
            case 'Q':
                return colorOfDragPiece ? WHITE_QUEEN : BLACK_QUEEN;
            }
        }
        std::cout << ">> Invalid input, please try again" << std::endl
                  << std::endl;
    }
}

void Interface::initiate()
{
    if (!menu())
    {
        return;
    }
    game->state->setStateFromFEN(STANDARD_OPENING_FEN);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Failed to initiate SDL: " << SDL_GetError() << std::endl
                  << std::endl;
        return;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "Failed to initiate IMG: " << SDL_GetError() << std::endl
                  << std::endl;
        return;
    }

    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    renderer = SDL_CreateRenderer(window, -1, 0);

    loadPieces();

    running = true;
}

void Interface::eventHandler(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            resizeWindow(event.window.data2, event.window.data1);
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            if (dragPieceByte == NO_PIECE)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                pickupDragPiece(mouseX, mouseY);
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (dragPieceByte != NO_PIECE)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            releaseDragPiece(mouseX, mouseY);
        }
        break;
    case SDL_QUIT:
        running = false;
        break;
    default:
        break;
    }
}

void Interface::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderBoard();
    renderState();

    if (dragPieceByte != NO_PIECE)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        renderDragPiece(mouseX, mouseY);
    }

    SDL_RenderPresent(renderer);
}

void Interface::loadPieces()
{
    char *PNGLocations[NUM_OF_PIECES];
    PNGLocations[0] = (char *)DARK_PAWN_IMG;
    PNGLocations[1] = (char *)LIGHT_PAWN_IMG;
    PNGLocations[2] = (char *)DARK_KNIGT_IMG;
    PNGLocations[3] = (char *)LIGHT_KNIGHT_IMG;
    PNGLocations[4] = (char *)DARK_BISHOP_IMG;
    PNGLocations[5] = (char *)LIGHT_BISHOP_IMG;
    PNGLocations[6] = (char *)DARK_ROOK_IMG;
    PNGLocations[7] = (char *)LIGHT_ROOK_IMG;
    PNGLocations[8] = (char *)DARK_QUEEN_IMG;
    PNGLocations[9] = (char *)LIGHT_QUEEN_IMG;
    PNGLocations[10] = (char *)DARK_KING_IMG;
    PNGLocations[11] = (char *)LIGHT_KING_IMG;

    for (int i = 0; i < NUM_OF_PIECES; i++)
    {
        SDL_Surface *image = IMG_Load(PNGLocations[i]);
        if (!image)
        {
            std::cerr << "Failed to load image from: " << PNGLocations[i] << std::endl;
        }
        pieces[i] = SDL_CreateTextureFromSurface(renderer, image);
        if (pieces[i] == NULL)
        {
            std::cerr << "Failed to create texture from image: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(image);
    }
}

SDL_Texture *Interface::getTexturePieceFromByte(uint8_t byte)
{
    // Mapping the byte for a piece to its corresponding index in the 'pieces' array
    switch (byte)
    {
    case BLACK_PAWN:
        return pieces[0];
    case WHITE_PAWN:
        return pieces[1];
    case BLACK_KNIGHT:
        return pieces[2];
    case WHITE_KNIGHT:
        return pieces[3];
    case BLACK_BISHOP:
        return pieces[4];
    case WHITE_BISHOP:
        return pieces[5];
    case BLACK_ROOK:
        return pieces[6];
    case WHITE_ROOK:
        return pieces[7];
    case BLACK_QUEEN:
        return pieces[8];
    case WHITE_QUEEN:
        return pieces[9];
    case BLACK_KING:
        return pieces[10];
    default:
        return pieces[11];
    }
}

void Interface::renderBoard()
{
    squareEdge = (screenHeight * (screenHeight < screenWidth) + screenWidth * (screenHeight >= screenWidth)) / BOARD_LENGTH;

    boardStartingX = ((screenWidth - (squareEdge * BOARD_LENGTH)) / 2) * (screenWidth > squareEdge);
    boardStartingY = ((screenHeight - (squareEdge * BOARD_LENGTH)) / 2) * (screenHeight > squareEdge);

    int pos = 0;
    int i = 0;
    for (int y = BOARD_LENGTH - 1; y >= 0; y--)
    {
        for (int x = 0; x < BOARD_LENGTH; x++)
        {
            SDL_Rect rect;
            rect.w = squareEdge;
            rect.h = squareEdge;
            rect.x = boardStartingX + x * squareEdge;
            rect.y = boardStartingY + y * squareEdge;
            if ((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1))
            {
                if (dragPieceLegalMoves && i < dragPieceLegalMoves->size() && pos++ == dragPieceLegalMoves->at(i))
                {
                    i++;
                    SDL_SetRenderDrawColor(renderer, 228, 228, 150, 255);
                } // legal move square
                else
                {
                    SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
                }
            } // white plane
            else
            {
                if (dragPieceLegalMoves && i < dragPieceLegalMoves->size() && pos++ == dragPieceLegalMoves->at(i))
                {
                    i++;
                    SDL_SetRenderDrawColor(renderer, 108, 140, 26, 255);
                } // legal move square
                else
                {
                    SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
                }
            } // black plane
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Interface::renderState()
{
    int pos = 0;
    for (int y = BOARD_LENGTH - 1; y >= 0; y--)
    {
        for (int x = 0; x < BOARD_LENGTH; x++)
        {
            if (pos == dragPiecePos)
            {
                pos++;
                continue;
            }
            uint8_t byteFromByteBoard = game->state->getPieceAt(pos++);

            if (byteFromByteBoard == NO_PIECE)
            {
                continue;
            }

            SDL_Rect rect;
            rect.w = squareEdge;
            rect.h = squareEdge;
            rect.x = boardStartingX + x * squareEdge;
            rect.y = boardStartingY + y * squareEdge;
            SDL_RenderCopy(renderer, getTexturePieceFromByte(byteFromByteBoard), NULL, &rect);
        }
    }
}

void Interface::resizeWindow(int const height, int const width)
{
    screenHeight = height * (height > MIN_SCREEN_HEIGHT) + MIN_SCREEN_HEIGHT * (height <= MIN_SCREEN_HEIGHT);
    screenWidth = width * (width > MIN_SCREEN_WIDTH) + MIN_SCREEN_WIDTH * (width <= MIN_SCREEN_WIDTH);
}

void Interface::pickupDragPiece(int const mouseX, int const mouseY)
{
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;

    // if mouse coords are inside chess board, try to pick up a piece
    if ((squareXOfMouse >= 0 && squareXOfMouse < BOARD_LENGTH) && (squareYOfMouse >= 0 && squareYOfMouse < BOARD_LENGTH))
    {
        dragPieceTextureMouseX = (mouseX - boardStartingX) % squareEdge;
        dragPieceTextureMouseY = (mouseY - boardStartingY) % squareEdge;
        dragPiecePos = squareXOfMouse + (BOARD_LENGTH - squareYOfMouse - 1) * BOARD_LENGTH;
        dragPieceByte = game->state->getPieceAt(dragPiecePos);

        if (dragPieceByte == NO_PIECE || game->state->getTurn() != getColorOfPiece(dragPieceByte))
        {
            dragPieceByte = NO_PIECE;
            dragPiecePos = -1;
            return;
        } // there is no piece at this square or it's of the opponent's color
        dragPieceLegalMoves = game->getLegalMoves(dragPieceByte, dragPiecePos);
    }
}

void Interface::renderDragPiece(int const mouseX, int const mouseY)
{
    SDL_Rect rect;
    rect.w = squareEdge;
    rect.h = squareEdge;
    rect.x = mouseX - dragPieceTextureMouseX;
    rect.y = mouseY - dragPieceTextureMouseY;

    // Conditions for preventing dragging outside of chess board
    if (rect.x < boardStartingX - (squareEdge / 2))
    {
        rect.x = boardStartingX - (squareEdge / 2);
    }
    else if (rect.x > (boardStartingX + BOARD_LENGTH * squareEdge - (squareEdge / 2)))
    {
        rect.x = boardStartingX + BOARD_LENGTH * squareEdge - (squareEdge / 2);
    }
    if (rect.y < boardStartingY - (squareEdge / 2))
    {
        rect.y = boardStartingY - (squareEdge / 2);
    }
    else if (rect.y > (boardStartingY + BOARD_LENGTH * squareEdge - (squareEdge / 2)))
    {
        rect.y = boardStartingY + BOARD_LENGTH * squareEdge - (squareEdge / 2);
    }

    SDL_RenderCopy(renderer, getTexturePieceFromByte(dragPieceByte), NULL, &rect);
}

void Interface::releaseDragPiece(int const mouseX, int const mouseY)
{
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdge;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdge;
    int const newDragPiecePos = squareXOfMouse + (BOARD_LENGTH - squareYOfMouse - 1) * BOARD_LENGTH;

    for (int i = 0; i < dragPieceLegalMoves->size(); i++)
    {
        if (dragPieceLegalMoves->at(i) == newDragPiecePos)
        {
            if (dragPieceByte == WHITE_PAWN && newDragPiecePos >= (BOARD_SIZE - BOARD_LENGTH))
            {
                game->state->movePiece(menuPawnPromotion(), dragPiecePos, newDragPiecePos);
            } // pawn promotion (for now only queen)
            else if (dragPieceByte == BLACK_PAWN && newDragPiecePos < BOARD_LENGTH)
            {
                game->state->movePiece(menuPawnPromotion(), dragPiecePos, newDragPiecePos);
            } // pawn promotion (for now only queen)
            else
            {
                game->state->movePiece(dragPieceByte, dragPiecePos, newDragPiecePos);
            }
            game->state->passTurn();
            game->state->setSpecialMovesData(dragPieceByte, dragPiecePos, newDragPiecePos);

            break;
        }
    }

    // reset dragPiece variables
    dragPieceByte = NO_PIECE;
    dragPiecePos = -1;
    delete dragPieceLegalMoves;
    dragPieceLegalMoves = nullptr;
}