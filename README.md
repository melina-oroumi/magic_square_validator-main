# Magic Square Validator

## Overview

This program validates whether a given n x n matrix is a magic suare using multithreading (pthreads) in C.

A magic square requires:

- All rows rum to the same value (magic constant)
- All columns sum to the same value
- Both diagonals sum to the same value
- All values are unique and within the range 1 to n^2

## Features

Uses 4 concurrent threads: row, column, diagonal, and uniqueness validation. They maintain validation results using status arrays, generate a detailed report of all checks, and print a final score.

## How to compile

make

## How to run with an input file

make run FILE=input{#}.txt

# Clean

make clean

## Notes

- The program uses sleep(1) in each thread to simulate processing delay
- Thread execution order does not result in a logically-accurate score, but validation results are correct

## Author

Name: Melina Hosseinzadeh Oroumi
Student ID: 22598396
Date: 25/04/2026