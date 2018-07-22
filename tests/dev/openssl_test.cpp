#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"
#include "colors.hpp"

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

// �ݹ� �Լ�
static void genrsa_cb( int p, int n, void *arg )
{
	char c = '*';

	// prime number e �� ���� ������ ǥ��
	if( p == 0 ) c = '.';
	if( p == 1 ) c = ' + ';
	if( p == 2 ) c = '*';
	if( p == 3 ) c = 'n';

	printf( "%c", c );
}

#define	_color(cr)		(bryllite::colors::set(bryllite::colors::##cr))

// openssl test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	bryllite::clog << _color(yellow) << "Hello, Bryllite!" ;
	bryllite::clog << _color(gray) << "Welcome" << "\n"  ;

	std::cout << "what color is it?" << std::endl;
	return 0;
/*
	RSA* key;
	unsigned char cipher_text[ 256 ];
	unsigned char plain_text_sender[] = "This is RSA Test Program" ;
	unsigned char plain_text_receiver[ 256 ];
	unsigned int num;

	key = RSA_new();
	key = RSA_generate_key( 1024, 65535, nullptr, nullptr );
	if ( RSA_check_key( key ) == 1 ) {
		log( "valid key" );

		log( "plain text=%s", plain_text_sender );
		num = RSA_public_encrypt( sizeof( plain_text_sender ) -1, plain_text_sender, cipher_text, key, RSA_PKCS1_PADDING );
		num = RSA_private_decrypt( num, cipher_text, plain_text_receiver, key, RSA_PKCS1_PADDING );
		plain_text_receiver[ num ] = 0;
		log( "plain text=%s", plain_text_receiver );

	} else log( "invalid key!" );

	RSA_free( key );
*/

	int keyLenInput = 512;    // Ű ����
//	char outPublicKeyFile[50];    // ����Ű�� ����
//	char outPrivateKeyFile[50];    // ����Ű�� ����

	char* outPublicKeyFile = "public.key";
	char* outPrivateKeyFile = "private.key";

	int isPem = true;    // PEM �������Ρ�

	BIO *publicOut = NULL;    // ����Ű�� ����Ǵ� ���� BIO
	BIO *privateOut = NULL;    // ����Ű�� ����Ǵ� ���� BIO
	BIO *bio_stdout = NULL;    // ȭ�鿡 ����� stdout BIO

	RSA *rsa = NULL;    // RSA ����ü

//	printf( "Ű ���� �Է� : " );    // Ű ���� �Է�
//	scanf( "%d", &keyLenInput );

//	printf( "������ ����Ű ���� �̸� �Է� : " );    // ����Ű�� ������ ���ϸ� �Է�
//	scanf( "%s", outPublicKeyFile );

//	printf( "������ ����Ű ���� �̸� �Է� : " );    // ����Ű�� ������ ���ϸ� �Է�
//	scanf( "%s", outPrivateKeyFile );

	// ǥ�� ȭ�� ��� BIO ����
//	if( ( bio_stdout = BIO_new( BIO_s_file() ) ) != NULL ) {
//		BIO_set_fp( bio_stdout, stdout, BIO_NOCLOSE | BIO_FP_TEXT );
//	}

	//����Ű�� ������ ���� BIO ����
	if( ( publicOut = BIO_new( BIO_s_file() ) ) == NULL ) {
		printf( "BIO ���� ����. %s", outPublicKeyFile );
		exit( 1 );
	}

	// ����Ű�� ������ ���� BIO ����
	if( ( privateOut = BIO_new( BIO_s_file() ) ) == NULL ) {
		printf( "BIO ���� ����. %s", outPublicKeyFile );
		exit( 1 );
	}

	// ���� BIO�� �ش� ����Ű ������ ���� ����
	if( BIO_write_filename( publicOut, outPublicKeyFile ) <= 0 ) {
		printf( "BIO ���� ����. %s", outPublicKeyFile );
		exit( 1 );
	}

	// ���� BIO�� �ش� ����Ű ������ ���� ����
	if( BIO_write_filename( privateOut, outPrivateKeyFile ) <= 0 ) {
		printf( "BIO ���� ����. %s", outPrivateKeyFile );
		exit( 1 );
	}

	RAND_status();    // seed ����, ����

					  // Ű ����
	rsa = RSA_generate_key( keyLenInput, RSA_F4, genrsa_cb, NULL );

	if( isPem ) {
		// PEM �������� ǥ�� ȭ�� ��� BIO�� ����Ű ���
		if( !PEM_write_bio_RSA_PUBKEY( bio_stdout, rsa ) ) {
			printf( "PEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}
		printf( "\n\n" );

		// PEM �������� ǥ�� ȭ�� ��� BIO�� ����Ű ���
		if( !PEM_write_bio_RSAPrivateKey( bio_stdout, rsa, NULL, NULL, 0, NULL, NULL ) ) {
			printf( "PEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}

		// PEM �������� ���� BIO�� ����Ű ���
		if( !PEM_write_bio_RSA_PUBKEY( publicOut, rsa ) ) {
			printf( "PEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}

		// PEM �������� ���� BIO�� ����Ű ���
		if( !PEM_write_bio_RSAPrivateKey( privateOut, rsa, NULL, NULL, 0, NULL, NULL ) ) {
			printf( "PEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}
	}
	else {    // ���� DEM �������� Ű ���� ��� �Ѵٸ�..
		printf( "DEM PUBLIC KEY\n" );

		// DEM �������� ǥ�� ȭ�� ��� BIO�� ����Ű ���
		if( !i2d_RSA_PUBKEY_bio( bio_stdout, rsa ) ) {
			printf( "DEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}

		printf( "\n\n" );
		printf( "DER PRIVATE KEY\n" );
		printf( "\n" );

		// DEM �������� ǥ�� ȭ�� ��� BIO�� ����Ű ���
		if( !i2d_RSAPrivateKey_bio( bio_stdout, rsa ) ) {
			printf( "DEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}

		// DEM �������� ���� BIO�� ����Ű ���
		if( !i2d_RSA_PUBKEY_bio( publicOut, rsa ) ) {
			printf( "DEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}

		// DEM �������� ���� BIO�� ����Ű ���
		if( !i2d_RSAPrivateKey_bio( privateOut, rsa ) ) {
			printf( "DEM ���� ���� ���� %s", outPrivateKeyFile );
			exit( 1 );
		}
	}

	// RSA ����ü �޸𸮿��� ����
	if( rsa != NULL )
		RSA_free( rsa );

	// BIO�� �޸𸮿��� ����
	if( publicOut != NULL ) BIO_free_all( publicOut );
	if( privateOut != NULL ) BIO_free_all( privateOut );


	return 0 ;
}

