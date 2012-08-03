Nutella -- A simple gnutella-like p2p file sharing and streaming application
=============

Preliminary
------------
Your .nutella file should have the locations of all your movie files.
The first line should have the number of movies in your collection. 
Every line after that should be formatted as such:
    <name of moviie>    <file location>

e.g:
    2
    matrix      movies/matrix.txt
    starwars    movies/starwars.txt


Usage
------------
To compile:
    ~$ 'make clean'
    ~$ 'make'

To run the client:
    ~$ './nclient'

To run the server:
    ~$ './nserver'

