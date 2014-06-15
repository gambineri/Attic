/* crypt 21/10/1999
 *
 * utility wrap for the public domain DES implementation by
 * mr. Outerbridge
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
unsigned short itxEncrypt(
	unsigned char* DESKey,
	unsigned char* Whitenings,
	unsigned char* source,
	unsigned char* destination);

unsigned short itxDecrypt(
	unsigned char* DESKey,
	unsigned char* Whitenings,
	unsigned char* source,
	unsigned char* destination);
