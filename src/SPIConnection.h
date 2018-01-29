//     Shift Register LED Matrix Project - SPIConnection
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
// 
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
#ifndef __SPICONNECTION_H__
#define __SPICONNECTION_H__

class SPIConnection {
	int _slavePin;
	unsigned long _maxSPISpeed;
	
public:
	SPIConnection(
			int slavePin = 10,
			unsigned long maxSPISpeed = 18000000
		);
	virtual ~SPIConnection();
	
	void setup();
	
	void startTransaction(void);
	void transmitByte(unsigned char data);
	void endTransaction(void);
};

#endif //__SPICONNECTION_H__
