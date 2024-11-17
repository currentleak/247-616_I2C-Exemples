#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> //for IOCTL defs
#include <fcntl.h>

#include "VL6180x.h"
#include "VL6180x.c"
#include "MPU9250.h"
#include "MPU9250.c"

#define I2C_FICHIER_BUS_EXTERNE "/dev/i2c-1" // fichier Linux representant le BUS #1
#define I2C_FICHIER_BUS_INTERNE "/dev/i2c-2" // fichier Linux representant le BUS #2

int open_I2C_bus(char*, int);

int main()
{
	/// ouverture du port de communication pour le capteur MPU9250, bus I2C interne
	int fdPortI2C_Interne = open_I2C_bus(I2C_FICHIER_BUS_INTERNE, I2C_ADRESSE_MPU9250);
	if(fdPortI2C_Interne < 0)
	{
		return -1;
	}
	/// ouverture du port de communication pour le capteur VL6180x, bus I2C externe
	int fdPortI2C_Externe = open_I2C_bus(I2C_FICHIER_BUS_EXTERNE, I2C_ADRESSE_VL6180x);
	if(fdPortI2C_Externe < 0)
	{
		return -1;
	}

	printf("ID MPU9250 = %#04X\n", MPU9250_lire_ID(fdPortI2C_Interne)); // no error mngt
	printf("ID VL6180x = %#04X\n", VL6180x_lire_ID(fdPortI2C_Externe)); // no error mngt

	if(VL6180x_initialiser(fdPortI2C_Externe) < 0)
	{
		printf("erreur VL6180x Initialiser\n");
		return -1;
	}

	float distance = 0.0;
	while(1)
	{
		if(VL6180x_lireUneDistance(fdPortI2C_Externe, &distance) < 0 )
		{
			printf("erreur VL6180x Lire distance\n");
			return -1;
		}
		else
		{
			printf("d=%2.5f\n", distance);
		}
	}

	close(fdPortI2C_Interne);
	close(fdPortI2C_Externe);
	return 0;
}

int open_I2C_bus(char* port, int adresse_I2C)
{
	int fdPortI2C;  // file descriptor I2C

	// Initialisation du port I2C, 
	fdPortI2C = open(port, O_RDWR); // ouverture du 'fichier', création d'un 'file descriptor' vers le port I2C
	if(fdPortI2C == -1)
	{
		printf("erreur: ouverture du port I2C\n");
		return -1;
	}
	if(ioctl(fdPortI2C, I2C_SLAVE_FORCE, adresse_I2C) < 0)  // Liaison de l'adresse I2C au fichier (file descriptor) du bus I2C
	{ 	// I2C_SLAVE_FORCE if it is already in use by a driver (i2cdetect : UU)
		printf("erreur: adresse du device I2C\n");
		close(fdPortI2C);
		return -1;
	}
	return fdPortI2C;
}
