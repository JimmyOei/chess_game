#include "../include/interface.h"

Interface::Interface() : window(nullptr),
                         renderer(nullptr),
                         screenWidth(STARTING_SCREEN_WIDTH),
                         screenHeight(STARTING_SCREEN_HEIGHT),
                         squareEdgeLength(0),
                         boardStartingX(0),
                         boardStartingY(0),
                         running(false),
                         gameOver(false),
                         dragPiece(Piece::Type::BLANK),
                         dragPiecePos(Position(-1)),
                         dragPieceLegalMoves(std::vector<Move>()),
                         game(std::make_unique<Game>()),
                         playerWhite(std::make_unique<PlayerHuman>()),
                         playerBlack(std::make_unique<PlayerEngineRandom>())
{
}

Interface::~Interface()
{
    /* Destroy window */
    if (window)
    {
        SDL_DestroyWindow(window);
    }

    /* Destroy renderer */
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    /* Destroy all SDL textures */
    for (auto &[piece, texture] : texturePieces)
    {
        SDL_DestroyTexture(texture);
    }

    /* Quit IMG and SDL */
    IMG_Quit();
    SDL_Quit();
};

bool Interface::isRunning()
{
    return running;
}

bool Interface::isGameOver()
{
    return gameOver;
}

void Interface::menuGamemode(Piece::Color const color)
{
    while (true)
    {
        std::string playerInput;
        if (color == Piece::Color::WHITE)
        {
            std::cout << ">> Input the player for white" << std::endl;
        }
        else
        {
            std::cout << ">> Input the player for black" << std::endl;
        }
        std::cout << ">> \"1\": human player | \"2\": random move engine" << std::endl
                  << std::endl;
        getline(std::cin, playerInput);
        std::cout << std::endl;
        if (playerInput.length() == 1)
        {
            switch (playerInput[0])
            {
            case '1':
                color == Piece::Color::WHITE ? playerWhite = std::make_unique<PlayerHuman>() : playerBlack = std::make_unique<PlayerHuman>();
                return;
            case '2':
                color == Piece::Color::WHITE ? playerWhite = std::make_unique<PlayerEngineRandom>() : playerBlack = std::make_unique<PlayerEngineRandom>();
                return;
            }
        }
        std::cerr << ">> Invalid input, please try again" << std::endl
                  << std::endl;
    }
}

void Interface::menuFEN()
{
    // while (true)
    // {
    //     std::string FEN;
    //     std::cout << ">> Input a FEN-notation for the game or \"default\" for the standard opening position" << std::endl
    //               << ">> Example of expected input: \"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3\"" << std::endl
    //               << std::endl;
    //     getline(std::cin, FEN);
    //     std::cout << std::endl;
    //     if (FEN == "default")
    //     {
    //         FEN = STANDARD_OPENING_FEN;
    //     }
    //     if (game->state->setStateFromFEN(FEN))
    //     {
    //         return;
    //     } // correct FEN notation
    //     std::cerr << ">> The given FEN-notation is invalid: " << FEN << std::endl
    //               << ">> Please try again " << std::endl
    //               << std::endl;
    // }
}

bool Interface::menu()
{
    while (true)
    {
        std::string input;
        std::cout << ">> What would you like to do? (Input a letter)" << std::endl
                  << "\"S\": Start the Chess game" << std::endl
                  << "\"P\": Change the Player types" << std::endl
                  << "\"Q\": Quit program" << std::endl
                  << std::endl;
        getline(std::cin, input);
        std::cout << std::endl;
        if (input.length() == 1)
        {
            switch (input[0])
            {
            case 'S':
            case 's':
                std::cout << "The game will begin. Good luck!" << std::endl
                          << std::endl;
                return true;
            case 'P':
            case 'p':
                menuGamemode(Piece::Color::WHITE);
                menuGamemode(Piece::Color::BLACK);
                break;
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

Piece::Type Interface::menuPawnPromotion()
{
    bool const colorOfDragPiece = getColorOfPiece(dragPiece);
    while (true)
    {
        std::string input;
        std::cout << std::endl
                  << ">> Input the first letter of the piece you would like to promote your pawn to" << std::endl
                  << ">> \"q\": queen | \"r\": rook | \"b\": bishop | \"k\": knight" << std::endl
                  << std::endl;
        getline(std::cin, input);
        if (input.length() == 1)
        {
            switch (input[0])
            {
            case 'k':
            case 'K':
                return colorOfDragPiece ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT;
            case 'b':
            case 'B':
                return colorOfDragPiece ? Piece::WHITE_BISHOP : Piece::BLACK_BISHOP;
            case 'r':
            case 'R':
                return colorOfDragPiece ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
            case 'q':
            case 'Q':
                return colorOfDragPiece ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN;
            }
        }
        std::cout << ">> Invalid input, please try again" << std::endl
                  << std::endl;
    }
    return colorOfDragPiece ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN;
}

void Interface::initiate()
{
    if (!menu())
    {
        return;
    }

    /* Initiate SDL */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Failed to initiate SDL: " << SDL_GetError() << std::endl
                  << std::endl;
        return;
    }

    /* Initiate SDL_Img */
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "Failed to initiate IMG: " << SDL_GetError() << std::endl
                  << std::endl;
        return;
    }

    /* Set up game window */
    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);
    renderer = SDL_CreateRenderer(window, -1, 0);

    loadTexturePieces();

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
        if (event.button.button == SDL_BUTTON_LEFT && !gameOver)
        {
            if (dragPiece == Piece::Type::BLANK)
            {
                std::cout << "Picking up piece" << std::endl;
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                pickupDragPiece(mouseX, mouseY);
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (dragPiece != Piece::Type::BLANK)
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

    if (dragPiece != Piece::Type::BLANK)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        renderDragPiece(mouseX, mouseY);
    }

    SDL_RenderPresent(renderer);
}

void Interface::loadTexturePieces()
{
    std::unordered_map<Piece::Type, char *> PNGLocations;
    PNGLocations[Piece::Type::BLACK_PAWN] = (char *)DARK_PAWN_IMG;
    PNGLocations[Piece::Type::WHITE_PAWN] = (char *)LIGHT_PAWN_IMG;
    PNGLocations[Piece::Type::BLACK_KNIGHT] = (char *)DARK_KNIGT_IMG;
    PNGLocations[Piece::Type::WHITE_KNIGHT] = (char *)LIGHT_KNIGHT_IMG;
    PNGLocations[Piece::Type::BLACK_BISHOP] = (char *)DARK_BISHOP_IMG;
    PNGLocations[Piece::Type::WHITE_BISHOP] = (char *)LIGHT_BISHOP_IMG;
    PNGLocations[Piece::Type::BLACK_ROOK] = (char *)DARK_ROOK_IMG;
    PNGLocations[Piece::Type::WHITE_ROOK] = (char *)LIGHT_ROOK_IMG;
    PNGLocations[Piece::Type::BLACK_QUEEN] = (char *)DARK_QUEEN_IMG;
    PNGLocations[Piece::Type::WHITE_QUEEN] = (char *)LIGHT_QUEEN_IMG;
    PNGLocations[Piece::Type::BLACK_KING] = (char *)DARK_KING_IMG;
    PNGLocations[Piece::Type::WHITE_KING] = (char *)LIGHT_KING_IMG;

    for (auto &[piece, PNGLocation] : PNGLocations)
    {
        SDL_Surface *image = IMG_Load(PNGLocation);
        if (!image)
        {
            std::cerr << "Failed to load image from: " << PNGLocation << std::endl;
            throw std::invalid_argument("Failed to load image.");
        }
        texturePieces[piece] = SDL_CreateTextureFromSurface(renderer, image);
        if (texturePieces[piece] == nullptr)
        {
            std::cerr << "Failed to create texture from image: " << SDL_GetError() << std::endl;
            throw std::invalid_argument("Failed to create texture from image.");
        }
        SDL_FreeSurface(image);
    }
}

SDL_Texture *Interface::getTexturePiece(Piece::Type piece)
{
    if (texturePieces.find(piece) == texturePieces.end())
    {
        throw std::invalid_argument("Failed to find texture for piece.");
    }

    return texturePieces[piece];
}

void Interface::handlePlayerTurn()
{
    if (game->isGameOver())
    {
        std::cout << "Checkmate! " << ((game->getTurn() == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE) << " won." << std::endl;
        gameOver = true;
        return;
    }
    if (game->getTurn() == Piece::Color::WHITE)
    {
        if (!playerWhite->isHuman())
        {
            Move move = playerWhite->getMove(game.get());
            game->makeMove(move);
        }
    }
    else
    {
        if (!playerBlack->isHuman())
        {
            Move move = playerBlack->getMove(game.get());
            game->makeMove(move);
        }
    }
}

void Interface::renderBoard()
{
    // Length of an edge of a square on the board
    squareEdgeLength = (screenHeight * (screenHeight < screenWidth) + screenWidth * (screenHeight >= screenWidth)) / BOARD_LENGTH;

    // Top left square position of the board on the window
    boardStartingX = ((screenWidth - (squareEdgeLength * BOARD_LENGTH)) / 2) * (screenWidth > squareEdgeLength);
    boardStartingY = ((screenHeight - (squareEdgeLength * BOARD_LENGTH)) / 2) * (screenHeight > squareEdgeLength);

    int pos = 0;
    for (int y = BOARD_LENGTH - 1; y >= 0; y--)
    {
        for (int x = 0; x < BOARD_LENGTH; x++)
        {
            SDL_Rect rect;
            rect.w = squareEdgeLength;
            rect.h = squareEdgeLength;
            rect.x = boardStartingX + x * squareEdgeLength;
            rect.y = boardStartingY + y * squareEdgeLength;
            if ((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1))
            {
                /* Painting the color of the white square */
                if (std::find_if(dragPieceLegalMoves.begin(), dragPieceLegalMoves.end(), [pos](const Move &move)
                                 { return move.to == pos; }) != dragPieceLegalMoves.end())
                {
                    SDL_SetRenderDrawColor(renderer, 228, 228, 150, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
                }
            }
            else
            {
                /* Painting the color of the black square */
                if (std::find_if(dragPieceLegalMoves.begin(), dragPieceLegalMoves.end(), [pos](const Move &move)
                                 { return move.to == pos; }) != dragPieceLegalMoves.end())
                {
                    SDL_SetRenderDrawColor(renderer, 108, 140, 26, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
                }
            }
            pos++;
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
            Piece::Type piece = game->getPieceAtPos(pos++);

            if (piece == Piece::Type::BLANK)
            {
                continue;
            }

            SDL_Rect rect;
            rect.w = squareEdgeLength;
            rect.h = squareEdgeLength;
            rect.x = boardStartingX + x * squareEdgeLength;
            rect.y = boardStartingY + y * squareEdgeLength;
            SDL_RenderCopy(renderer, getTexturePiece(piece), NULL, &rect);
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
    logIt(LogLevel::DEBUG) << "Picking up piece at mouse coordinates (x, y): (" << mouseX << ", " << mouseY << ")";
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdgeLength;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdgeLength;

    // if mouse coords are inside chess board, try to pick up a piece
    if ((squareXOfMouse >= 0 && squareXOfMouse < BOARD_LENGTH) && (squareYOfMouse >= 0 && squareYOfMouse < BOARD_LENGTH))
    {
        dragPieceTextureMouseX = (mouseX - boardStartingX) % squareEdgeLength;
        dragPieceTextureMouseY = (mouseY - boardStartingY) % squareEdgeLength;
        dragPiecePos = squareXOfMouse + (BOARD_LENGTH - squareYOfMouse - 1) * BOARD_LENGTH;
        if (!dragPiecePos.isValid())
        {
            dragPiece = Piece::Type::BLANK;
            dragPiecePos = Position(-1);
            return;
        }
        dragPiece = game->getPieceAtPos(dragPiecePos);

        if (dragPiece == Piece::Type::BLANK || game->getTurn() != Piece::getColorOfPiece(dragPiece))
        {
            dragPiece = Piece::Type::BLANK;
            dragPiecePos = -1;
            return;
        } // there is no piece at this square or it's of the opponent's color
        dragPieceLegalMoves = game->getLegalMovesForPos(dragPiecePos);
    }
    logIt(LogLevel::INFO) << "Picked up piece " << dragPiece << " at " << dragPiecePos;
}

void Interface::renderDragPiece(int const mouseX, int const mouseY)
{
    SDL_Rect rect;
    rect.w = squareEdgeLength;
    rect.h = squareEdgeLength;
    rect.x = mouseX - dragPieceTextureMouseX;
    rect.y = mouseY - dragPieceTextureMouseY;

    // Conditions for preventing dragging outside of chess board
    if (rect.x < boardStartingX - (squareEdgeLength / 2))
    {
        rect.x = boardStartingX - (squareEdgeLength / 2);
    }
    else if (rect.x > (boardStartingX + BOARD_LENGTH * squareEdgeLength - (squareEdgeLength / 2)))
    {
        rect.x = boardStartingX + BOARD_LENGTH * squareEdgeLength - (squareEdgeLength / 2);
    }
    if (rect.y < boardStartingY - (squareEdgeLength / 2))
    {
        rect.y = boardStartingY - (squareEdgeLength / 2);
    }
    else if (rect.y > (boardStartingY + BOARD_LENGTH * squareEdgeLength - (squareEdgeLength / 2)))
    {
        rect.y = boardStartingY + BOARD_LENGTH * squareEdgeLength - (squareEdgeLength / 2);
    }

    SDL_RenderCopy(renderer, getTexturePiece(dragPiece), NULL, &rect);
}

void Interface::releaseDragPiece(int const mouseX, int const mouseY)
{
    logIt(LogLevel::DEBUG) << "Releasing piece " << dragPiece << " at mouse coordinates (x, y): (" << mouseX << ", " << mouseY << ")";
    int const squareXOfMouse = (mouseX - boardStartingX) / squareEdgeLength;
    int const squareYOfMouse = (mouseY - boardStartingY) / squareEdgeLength;
    int const newDragPiecePos = squareXOfMouse + (BOARD_LENGTH - squareYOfMouse - 1) * BOARD_LENGTH;

    for (auto &legalMove : dragPieceLegalMoves)
    {
        if (legalMove.to == newDragPiecePos)
        {
            if ((dragPiece == Piece::WHITE_PAWN && legalMove.to.getRow() == BOARD_LENGTH) ||
                (dragPiece == Piece::BLACK_PAWN && legalMove.to.getRow() == 1))
            {
                // Promotion
                const Move promotionMove(legalMove.from, legalMove.to, legalMove.piece, menuPawnPromotion());
                game->makeMove(promotionMove);
                logIt(LogLevel::INFO) << Piece::getColorOfPiece(legalMove.piece) << " made move " << promotionMove << " with promotion to " << promotionMove.promotionPiece;
                break;
            }

            game->makeMove(legalMove);
            logIt(LogLevel::INFO) << Piece::getColorOfPiece(legalMove.piece) << " made move " << legalMove;
            break;
        }
    }

    // reset dragPiece variables
    dragPiece = Piece::Type::BLANK;
    dragPiecePos = Position(-1);
    dragPieceLegalMoves.clear();
}