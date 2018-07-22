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

// 콜백 함수
static void genrsa_cb( int p, int n, void *arg )
{
	char c = '*';

	// prime number e 의 생성 과정을 표시
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

	int keyLenInput = 512;    // 키 길이
//	char outPublicKeyFile[50];    // 공개키를 저장
//	char outPrivateKeyFile[50];    // 개인키를 저장

	char* outPublicKeyFile = "public.key";
	char* outPrivateKeyFile = "private.key";

	int isPem = true;    // PEM 형식으로…

	BIO *publicOut = NULL;    // 공개키가 저장되는 파일 BIO
	BIO *privateOut = NULL;    // 개인키가 저장되는 파일 BIO
	BIO *bio_stdout = NULL;    // 화면에 출력할 stdout BIO

	RSA *rsa = NULL;    // RSA 구조체

//	printf( "키 길이 입력 : " );    // 키 길이 입력
//	scanf( "%d", &keyLenInput );

//	printf( "저장할 공개키 파일 이름 입력 : " );    // 공개키를 저장할 파일명 입력
//	scanf( "%s", outPublicKeyFile );

//	printf( "저장할 개인키 파일 이름 입력 : " );    // 개인키를 저장할 파일명 입력
//	scanf( "%s", outPrivateKeyFile );

	// 표준 화면 출력 BIO 생성
//	if( ( bio_stdout = BIO_new( BIO_s_file() ) ) != NULL ) {
//		BIO_set_fp( bio_stdout, stdout, BIO_NOCLOSE | BIO_FP_TEXT );
//	}

	//공개키를 저장할 파일 BIO 생성
	if( ( publicOut = BIO_new( BIO_s_file() ) ) == NULL ) {
		printf( "BIO 생성 에러. %s", outPublicKeyFile );
		exit( 1 );
	}

	// 개인키를 저장할 파일 BIO 생성
	if( ( privateOut = BIO_new( BIO_s_file() ) ) == NULL ) {
		printf( "BIO 생성 에러. %s", outPublicKeyFile );
		exit( 1 );
	}

	// 파일 BIO와 해당 공개키 저장할 파일 연결
	if( BIO_write_filename( publicOut, outPublicKeyFile ) <= 0 ) {
		printf( "BIO 생성 에러. %s", outPublicKeyFile );
		exit( 1 );
	}

	// 파일 BIO와 해당 개인키 저장할 파일 연결
	if( BIO_write_filename( privateOut, outPrivateKeyFile ) <= 0 ) {
		printf( "BIO 생성 에러. %s", outPrivateKeyFile );
		exit( 1 );
	}

	RAND_status();    // seed 생성, 공급

					  // 키 생성
	rsa = RSA_generate_key( keyLenInput, RSA_F4, genrsa_cb, NULL );

	if( isPem ) {
		// PEM 포멧으로 표준 화면 출력 BIO에 공개키 출력
		if( !PEM_write_bio_RSA_PUBKEY( bio_stdout, rsa ) ) {
			printf( "PEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}
		printf( "\n\n" );

		// PEM 포멧으로 표준 화면 출력 BIO에 개인키 출력
		if( !PEM_write_bio_RSAPrivateKey( bio_stdout, rsa, NULL, NULL, 0, NULL, NULL ) ) {
			printf( "PEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}

		// PEM 포멧으로 파일 BIO에 공개키 출력
		if( !PEM_write_bio_RSA_PUBKEY( publicOut, rsa ) ) {
			printf( "PEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}

		// PEM 포멧으로 파일 BIO에 개인키 출력
		if( !PEM_write_bio_RSAPrivateKey( privateOut, rsa, NULL, NULL, 0, NULL, NULL ) ) {
			printf( "PEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}
	}
	else {    // 만약 DEM 포멧으로 키 쌍을 출력 한다면..
		printf( "DEM PUBLIC KEY\n" );

		// DEM 포멧으로 표준 화면 출력 BIO에 공개키 출력
		if( !i2d_RSA_PUBKEY_bio( bio_stdout, rsa ) ) {
			printf( "DEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}

		printf( "\n\n" );
		printf( "DER PRIVATE KEY\n" );
		printf( "\n" );

		// DEM 포멧으로 표준 화면 출력 BIO에 개인키 출력
		if( !i2d_RSAPrivateKey_bio( bio_stdout, rsa ) ) {
			printf( "DEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}

		// DEM 포멧으로 파일 BIO에 공개키 출력
		if( !i2d_RSA_PUBKEY_bio( publicOut, rsa ) ) {
			printf( "DEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}

		// DEM 포멧으로 파일 BIO에 개인키 출력
		if( !i2d_RSAPrivateKey_bio( privateOut, rsa ) ) {
			printf( "DEM 파일 생성 에러 %s", outPrivateKeyFile );
			exit( 1 );
		}
	}

	// RSA 구조체 메모리에서 삭제
	if( rsa != NULL )
		RSA_free( rsa );

	// BIO를 메모리에서 삭제
	if( publicOut != NULL ) BIO_free_all( publicOut );
	if( privateOut != NULL ) BIO_free_all( privateOut );


	return 0 ;
}

