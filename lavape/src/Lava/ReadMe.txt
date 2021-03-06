========================================================================
       MICROSOFT FOUNDATION CLASS BIBLIOTHEK : Lava
========================================================================


Diese Lava-Anwendung hat der Klassen-Assistent f�r Sie erstellt. Diese Anwendung
zeigt nicht nur die prinzipielle Verwendung der Microsoft Foundation Classes, 
sondern dient auch als Ausgangspunkt f�r die Erstellung Ihrer eigenen DLLs.

Diese Datei enth�lt die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre Lava-Anwendung bilden.

Lava.h
    	Hierbei handelt es sich um die Haupt-Header-Datei der Anwendung. Diese enth�lt 
	andere projektspezifische Header (einschlie�lich Resource.h) und deklariert die
	Anwendungsklasse CLavaApp.

Lava.cpp
    	Hierbei handelt es sich um die Haupt-Quellcodedatei der Anwendung. Diese enth�lt die
    	Anwendungsklasse CLavaApp.

Lava.rc
	Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
	vom Programm verwendet werden. Sie enth�lt die Symbole, Bitmaps und Cursors, die im 
	Unterverzeichnis RES abgelegt sind. Diese Datei l�sst sich direkt im Microsoft
	Developer Studio bearbeiten.

res\Lava.ico
    	Dies ist eine Symboldatei, die als Symbol f�r die Anwendung verwendet wird. Dieses 
	Symbol wird durch die Haupt-Ressourcendatei Lava.rc eingebunden.

res\Lava.rc2
    	Diese Datei enth�lt Ressourcen, die nicht vom Microsoft Developer Studio bearbeitet wurden.
	In diese Datei werden alle Ressourcen abgelegt, die vom Ressourcen-Editor nicht bearbeitet 
	werden k�nnen.

Lava.reg
    	Dies ist eine .REG-Beispieldatei, die Ihnen anzeigt, welche Registrierungseinstellungen 
	durch die Programmumgebung festlegt werden. Diese k�nnen Sie als .REG-
    	-Datei f�r Ihre Anwendung festlegen, oder diese l�schen und die Standardregistrierung 
	RegisterShellFileTypes verwenden.

Lava.odl
    	Diese Datei enth�lt den Quellcode der Object Description Language (ODL) f�r die
    	Typbibliothek Ihrer Anwendung.

Lava.clw
    	Diese Datei enth�lt Informationen, mit denen der Klassen-Assistent bestehende
    	Klassen bearbeitet oder neue Klassen einf�gt. Au�erdem verwendet der Klassen-Assistent 
	diese Datei, um Informationen zu speichern, die er f�r das Erstellen und Bearbeiten von
	Nachrichtentabellen und Dialogfeld-Nachrichtentabellen sowie f�r das Erstellen von 
	Prototypen vom Member-Funktionen ben�tigt.

/////////////////////////////////////////////////////////////////////////////

F�r das Hauptfenster:

MainFrm.h, MainFrm.cpp
    	Diese Dateien enthalten die Frame-Klasse CMainFrame, die von
    	CMDIFrameWnd abgeleitet wurde und alle MDI-Frame-Merkmale steuert.

res\Toolbar.bmp
    	Mit dieser Bitmap-Datei werden nebeneinander angeordnete Bilder f�r 
	die Symbolleiste erstellt. Die beginnende Symbolleiste und Statusleiste
	wird in der Klasse CMainFrame konstruiert. Um der Symbolleiste
	weitere Schaltfl�chen hinzuzuf�gen, wird diese Symbolleisten-
	Bitmap zusammen mit dem Array in MainFrm.cpp bearbeitet.

res\IToolbar.bmp
	Mit dieser Bitmap-Datei werden nebeneinander angeordnete Bilder f�r 
	die Symbolleiste erstellt, wenn die Server-Anwendung innerhalb eines
	anderen Containers "vor Ort" aktiviert wird. Die Symbolleiste wird
	in der Klasse CInPlaceFrame konstruiert. Diese Bitmap ist der Bitmap in 
	res\Toolbar.bmp �hnlich, jedoch fehlen verschiedene Nicht-Server-Befehle.

/////////////////////////////////////////////////////////////////////////////

Der Klassen-Assistent erstellt einen Dokumenttyp und eine Ansicht(View):

LavaDoc.h, LavaDoc.cpp - das Dokument
    	Diese Dateien enthalten die Klasse CLavaDoc. Bearbeiten Sie diese Dateien,
  	um Ihre speziellen Dokumentdaten hinzuzuf�gen und das Speichern und Laden von 
	Dateien zu implementieren (mit Hilfe von CLavaDoc::Serialize).

LavaView.h, LavaView.cpp - die Ansicht des Dokuments
    	Diese Dateien enthalten die Klasse CLavaView.
    	CLavaView-Objekte werden verwendet, um CLavaDoc-Objekte anzuzeigen.

res\LavaDoc.ico
    	Dies ist eine Symboldatei, die als Symbol f�r untergeordnete MDI-Fenster 
	der Klasse CLavaDoc verwendet wird. Dieses Symbol wird durch die
	Haupt-Ressourcendatei Lava.rc eingebunden.

/////////////////////////////////////////////////////////////////////////////

Dar�ber hinaus hat der Klassen-Assistent OLE-spezifische Klassen erzeugt.

CntrItem.h, CntrItem.cpp - mit dieser Klasse werden OLE-Objekte
	manipuliert. Diese werden in der Regel durch die Klasse CLavaView 
	angezeigt und als Teil der Klasse CLavaDoc serialisiert.

SrvrItem.h, SrvrItem.cpp - diese Klasse dient der Verbindung
	der Klasse CLavaDoc mit dem OLE-System und stellt auf Wunsch eine 
	Verkn�pfung zu Ihrem Dokument zur Verf�gung.
IpFrame.h, IpFrame.cpp - diese Klasse ist abgeleitet
	von COleIPFrameWnd und steuert alle Rahmenmerkmale w�hrend der Vor-Ort-
	Aktivierung.

/////////////////////////////////////////////////////////////////////////////
Andere Standarddateien:

StdAfx.h, StdAfx.cpp
    	Mit diesen Dateien werden vorkompilierte Header-Dateien (PCH) mit der Bezeichnung 
	Lava.pch und eine vorkompilierte Typdatei mit der Bezeichnung StdAfx.obj
	erstellt.

Resource.h
    	Dies ist die Standard-Header-Datei, die neue Ressourcen-IDs definiert.
    	Microsoft Developer Studio liest und aktualisiert diese Datei.

/////////////////////////////////////////////////////////////////////////////
Weitere Hinweise:

Der Klassen-Assistent f�gt "ZU ERLEDIGEN:" im Quellcode ein, um die Stellen anzuzeigen, 
an denen Sie Erweiterungen oder Anpassungen vornehmen k�nnen.

Wenn Ihre Anwendung die MFC in einer gemeinsam genutzten DLL verwendet und Ihre Anwendung
eine andere als die aktuell auf dem Betriebssystem eingestellte Sprache verwendet, muss 
die entsprechend lokalisierte Ressource MFC40XXX.DLL von der Microsoft Visual C++ CD-ROM 
in das Verzeichnis system oder system32 kopiert und in MFCLOC.DLL umbenannt werden ("XXX" 
steht f�r die Abk�rzung der Sprache. So enth�lt beispielsweise MFC40DEU.DLL die ins Deutsche 
�bersetzten Ressourcen). Anderenfalls erscheinen einige Oberfl�chenelemente Ihrer Anwendung 
in der Sprache des Betriebssystems.

/////////////////////////////////////////////////////////////////////////////
