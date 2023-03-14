## Chess Game

This is a personal project. I am creating a chess game in C++ with SDL 2.0.


## TODO
- implement mate
- start and end interface (put menu inside interface class)
- special moves in isLegalMove: pawn promotion, castling, en passant
- highlight legal moves for drag piece
- isKingAttacked function: check "attack field" and can stop when king is attacked, keep track of king positions
- moving a piece: in interface just dont render dragged piece (instead of removing it from the board), create function to *try to* move a piece in class Game (which can be used to check if the king is moving) which will call the isLegalMove function and return false if not worked to update the rendering

*Game:*
- getPossibleMoves\[Pawn|Knight|Bishop|Rook|Queen|King\]
- getLegalMoves
- isKingAttacked
- MovePiece

*Interface:*
- pickupPiece
- renderLegalMoves
- releasePiece

*Ideas*
- random move opponent


## License

The chess pieces in this project are made by Cburnett and unchanged.
They are licensed with the Creative Commons Attribution-Share Alike 3.0 Unported license
and can be found in the following link: https://commons.wikimedia.org/wiki/Category:PNG_chess_pieces/Standard_transparent
and https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces.
