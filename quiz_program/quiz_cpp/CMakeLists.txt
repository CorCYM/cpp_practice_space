cmake_minimum_required(VERSION 3.16)

project(quiz_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(quiz_cpp
    main.cpp
    Question.cpp
    QuizGame.cpp
)

configure_file(questions.txt questions_new.txt COPYONLY)