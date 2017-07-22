//---------------------------------------------------------------------------
#ifndef classEcgH
#define classEcgH
//---------------------------------------------------------------------------
#include <classes.hpp>
//---------------------------------------------------------------------------
#include <libs/core/classBase.h>
#include <libs/core/classData.h>
//---------------------------------------------------------------------------
#include <libs/core/classRpeaks.h>
#include <libs/core/classQrs.h>
#include <libs/core/classHeartbeat.h>
//---------------------------------------------------------------------------
//! Kapselt ein EKG
/*! Die Klasse cEcg kapselt die Daten und Funktionalit�ten eines EKGs (welches
 *  zu einer bestimmten Person in einer bestimmten Lage (Position) geh�rt.
 *  Ein EKG hat:\r\n
 *  <ul><li>Daten (cData), die elektrischen Ausschl�ge
 *  <li>R-Peaks (cRpeaks) und QRS-Bereiche (cQrs)
 *  <li>einen Standardherzschlag
 */
class PACKAGE cEcg : public cBase
	{
public:
	cEcg();  //!< Konstruktor f�r cEcg
	~cEcg(); //!< Destruktor f�r cEcg

__property cData& data 		  = { read=get_data };  //!< Daten des EKGs (Ausschl�ge der leads)
__property cRpeaks& rpeaks	  = { read=get_rpeaks };//!< R-Peaks des EKG
__property cQrs& qrs 		  = { read=get_qrs };   //!< QRS-Bereiche des EKG
__property cHeartbeats& heart = { read=get_heart }; //!< Standardherzschlag des EKG

__property int number    = { read=get_number, write=set_number };
__property String file   = { read=get_file,   write=set_file };
__property TImage* image = { read=get_image,  write=set_image };

private:
	cData*		fdata;
	cData&		get_data();

	cRpeaks*	frpeaks;
	cRpeaks&	get_rpeaks();

	cQrs*		fqrs;
	cQrs&		get_qrs();

	cHeartbeats*	fheart;
	cHeartbeats&	get_heart();

	//Hilfsvariablen, hier k�nnen Informationen hinterlegt werden, wenn mehrere
    //cEcg-Klassen in einerm Formular verwendet werden
	int         fNumber;  //zugeordnete Nummer
	bool		set_number(int nr);
	int         get_number();

	String      fEcgFile; //zu laden aus dieser Datei
	bool 		set_file(String file);
	String      get_file();

	TImage*     fImage;  //zugeordnetes Image
	bool        set_image(TImage* img);
    TImage*     get_image();

	};
//---------------------------------------------------------------------------
#endif
