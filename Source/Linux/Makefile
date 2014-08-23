# This code is part of DNSPing(Linux)
# DNSPing, Ping with DNS requesting.
# Copyright (C) 2014 Chengr28
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#Prepare
Target_Name = DNSPing
CC = g++
COM_FLAGS = -std=c++11 -O3 -fpic -c
LNK_FLAGS = -std=c++11 -O3 -fpic -lpthread -Xlinker
OBJ_DIR = ./Object/

all : Directory Target clean
Directory : 
		@echo Creating directory of object files.
		@-mkdir $(OBJ_DIR)

#Compile
Target : Terminal.o Protocol.o Process.o Main.o
		@echo Linking all object files...
		@$(CC) $(LNK_FLAGS) $(OBJ_DIR)*.o -o $(Target_Name)
Terminal.o : $(SRC_DIR)Terminal.cc
		@echo Compiling Terminal module...
		@$(CC) $(COM_FLAGS) ./Terminal.cc -o $(OBJ_DIR)Terminal.o
Protocol.o : $(SRC_DIR)Protocol.cc
		@echo Compiling Protocol module...
		@$(CC) $(COM_FLAGS) ./Protocol.cc -o $(OBJ_DIR)Protocol.o
Process.o : $(SRC_DIR)Process.cc
		@echo Compiling Process module...
		@$(CC) $(COM_FLAGS) ./Process.cc -o $(OBJ_DIR)Process.o
Main.o : $(SRC_DIR)Main.cc
		@echo Compiling Main module...
		@$(CC) $(COM_FLAGS) ./Main.cc -o $(OBJ_DIR)Main.o

#Cleanup
.PHONY : clean
clean : 
		@echo Cleaning...
		@-rm -r Object
