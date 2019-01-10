/*==================================================================/
Program obracający kostkę (sześcian) przy użyciu biblioteki OpenCV 	/
Autor: Bartosz Pacuła												/
Styczeń 2019r.														/
===================================================================*/

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"

#include "iostream"

using namespace std;
using namespace cv;

namespace kostka{

	class Sciana {
	public:
		int jestWidoczna; //flaga określająca czy ściana jest widoczna
		
		Sciana();
		void set(Vec4i); //ustawia wartości wektora kolejności wierzchołków
		Vec4i get(void); //pobiera wektor kolejności wierzchołków
		void fl(int); //ustawia wartość flagi widoczności
		void set_kolor(Scalar); //ustawia kolor ściany
		Scalar get_kolor(); //pobiera kolor ściany
		void setTekstura(Mat); //ustawia teksturę ściany
		Mat getTekstura(); //pobiera teksturę ściany
		Mat rysujTeksture(Scalar); //rysuje teksturę ściany o zadanym kolorze

	private:
		Vec4i kol_w; //kolejność wierzchołków
		Scalar kolor; //kolor ściany
		Mat tekstura; //tekstura ściany
	};


	Sciana::Sciana(void){
		kol_w=(0,1,2,3); //domyślna kolejność wierzchołków
		jestWidoczna=1; //domyślnie widoczna
		kolor=(255,255,255); //domyślny kolor biały (inicjalizacja) 
		Mat im(2,2,CV_8UC3,Scalar(255,255,255)); //tworzę początkowy obraz tekstury ściany (jednolicie biały) (mała tekstura początkowa, tylko do inicjalizacji)
		tekstura=im.clone(); //kopiuję wstępnie utworzoną teksturę do tekstury ściany
	}


	void Sciana::set(Vec4i kolejnosc){
		kol_w=kolejnosc;
	}

	Vec4i Sciana::get(void){
		return kol_w;
	}

	void Sciana::fl(int flaga){
		jestWidoczna=flaga;
	}

	void Sciana::set_kolor(Scalar k){
		kolor=k;
	}

	Scalar Sciana::get_kolor(){
		return kolor;
	}

	void Sciana::setTekstura(Mat im){
		tekstura=im.clone();
	}

	Mat Sciana::getTekstura(){
		return tekstura;
	}

	Mat Sciana::rysujTeksture(Scalar kolor){ //rysuje teksturę ściany o zadanym kolorze
		int i,j;
		int szer_krawedzi=5;
		Point2f p1,p2; //punkty końcowe szarych linii
		Scalar kolor_jasnoszary=Scalar(200,200,200); //na potrzeby rysowania jasnoszarych linii
		Scalar kolor_bialy=Scalar(255,255,255); //wartości BGR dla koloru białego
		Mat im(300,300,CV_8UC3,kolor_bialy); //tworzę obraz (najpierw ilość rzędów, później ilość kolumn, typ danych i ile kanałów oraz wartości jasności)
		int szer=(int)((im.cols-szer_krawedzi*4)/3.0); //szerokosc pojedynczego kwadratu
		int wys=szer; //wysokosc pojedynczego kwadratu
		Mat im_temp(szer,wys,CV_8UC3,kolor); //obraz kwadratu
		Rect prostokat; //ustalam ROI (region of interest)
		
		//rysowanie kwadratów--------------------------------	
		for(j=0;j<3;j++)
			for(i=0;i<3;i++)
			{
				prostokat=Rect(szer_krawedzi+i*(szer_krawedzi+szer),szer_krawedzi+j*(szer_krawedzi+wys),szer,wys);
				im_temp.copyTo(im(prostokat)); //wklejanie kwadratu do obrazu ściany
			}

		//rysowanie szarych linii
		for(i=0;i<3;i++)
		{
			p1=Point2f(szer_krawedzi+szer/(float)2.0+i*(szer_krawedzi+szer),(float)szer_krawedzi+2 /*2 - poprawka dla grubości linii*/); //dla linii pionowych 
			p2=Point2f(szer_krawedzi+szer/(float)2.0+i*(szer_krawedzi+szer),(float)im.rows-szer_krawedzi-2);

			line(im,p1,p2,kolor_jasnoszary,2 /*grubość linii*/,CV_AA /*typ: z antyaliasingiem*/,0 /*ilosc ułamkowych części współrzędnych punktu*/);

			p1=Point2f((float)szer_krawedzi+2,szer_krawedzi+szer/(float)2.0+i*(szer_krawedzi+szer)); //dla linii poziomych
			p2=Point2f((float)im.cols-szer_krawedzi-2,szer_krawedzi+szer/(float)2.0+i*(szer_krawedzi+szer));
		
			line(im,p1,p2,kolor_jasnoszary,2 /*grubość linii*/,CV_AA /*typ: z antyaliasingiem*/,0 /*ilosc ułamkowych części współrzędnych punktu*/);
		}

		return im;
	}


	class Szescian
	{
	public:
		Mat w; //wierzchołki sześcianu (macierz 8x3)
		int jestWidoczna[6]; //wektor flag oznaczających czy dana ściana tego szeszcianu jest widoczna

		Szescian(); //domyślny konstruktor
		Sciana get(int); //pobiera ścianę o danym numerze
	private:
		Sciana sc[6]; //sześć ścian sześcianu
	};


	Szescian::Szescian(void){ //inicjalizacja
		int i;
		w=(Mat_<double>(8,3) << //sześcian o środku w początku układu współrzędnych i szerokości równej 2,
		-1.0,1.0,-1.0,			//współrzędne kolejnych wierzchołków
		-1.0,-1.0,-1.0,
		1.0,-1.0,-1.0,
		1.0,1.0,-1.0,
		-1.0,1.0,1.0,
		-1.0,-1.0,1.0,
		1.0,-1.0,1.0,
		1.0,1.0,1.0);
		sc[0].set(Vec4i(0,1,2,3)); //ustalenie kolejności wierzchołków w danej ścianie
		sc[1].set(Vec4i(3,2,6,7));
		sc[2].set(Vec4i(7,6,5,4));
		sc[3].set(Vec4i(4,5,1,0));
		sc[4].set(Vec4i(4,0,3,7));
		sc[5].set(Vec4i(1,5,6,2));
		for(i=0;i<6;i++) jestWidoczna[i]=1; //ustawienie początkowe flag widoczności ścian

		//ustawienie początkowych kolorów ścian (kolejność kolorów BGR)
		sc[0].set_kolor(Scalar(255,128,128));
		sc[1].set_kolor(Scalar(128,255,128));
		sc[2].set_kolor(Scalar(128,128,255));
		sc[3].set_kolor(Scalar(255,255,128));
		sc[4].set_kolor(Scalar(128,255,255));
		sc[5].set_kolor(Scalar(255,128,255));

		//ustawienie początkowych tekstur poszczególnych ścian sześcianu
		for(i=0;i<6;i++)
			sc[i].setTekstura(sc[i].rysujTeksture(sc[i].get_kolor())); //przypisuję tekstury ścian
	}


	Sciana Szescian::get(int i){
		return (i>=0 && i<=5)?sc[i]:sc[0]; //przy okazji zabezpieczenie jeśli ktoś chce pobrać wartość spoza zakresu
	}

	//-------------------------------------------------Prototypy funkcji-------------------------------------------------------
	Mat obrot(Mat,double,double,double); //obrót obiektu w przestrzeni 3D
	void o_programie(void); //wyświetlenie informacji o programie
	void okreslWektNorm(Szescian&); //funkcja oblicza wektor normalny danej ściany i ustawia wartość flagi widoczności ściany
	Mat renderujPerspektywe(Szescian&,Szescian&); //tworzy obraz sześcianu z przekszałconymi przez perspektywę bitmapami
	void sklejObrazy(vector<Mat>&,Mat&,int); //łączenie obrazów poszczególnych ścian w obraz całościowy
	Mat updateView(Szescian&,double,double,double);  //aktualizacja obrazu (obrót kostki o kąt alfa, beta lub gamma i utworzenie 
													//obrazu; kąty wyrażone w stopniach)
	//-------------------------------------------------------------------------------------------------------------------------

	Mat obrot(Mat obiekt, double alfa, double beta, double gamma) //obrót obiektu w przestrzeni 3D
	{
		Mat Rx=Mat_<double>(3,3);
		Mat Ry=Mat_<double>(3,3);
		Mat Rz=Mat_<double>(3,3);

		Mat R=Mat_<double>(3,3); //zbiorcza macierz rotacji
		
		//inicjalizacja macierzy rotacji wokół osi x
		Rx.at<double>(0,0)=1.0;
		Rx.at<double>(0,1)=0.0;
		Rx.at<double>(0,2)=0.0;

		Rx.at<double>(1,0)=0.0;
		Rx.at<double>(1,1)=cos(alfa);
		Rx.at<double>(1,2)=-sin(alfa);

		Rx.at<double>(2,0)=0.0;
		Rx.at<double>(2,1)=sin(alfa);
		Rx.at<double>(2,2)=cos(alfa);
		//------------------------------------------
		//inicjalizacja macierzy rotacji wokół osi y
		Ry.at<double>(0,0)=cos(beta);
		Ry.at<double>(0,1)=0.0;
		Ry.at<double>(0,2)=sin(beta);

		Ry.at<double>(1,0)=0.0;
		Ry.at<double>(1,1)=1.0;
		Ry.at<double>(1,2)=0.0;

		Ry.at<double>(2,0)=-sin(beta);
		Ry.at<double>(2,1)=0.0;
		Ry.at<double>(2,2)=cos(beta);
		//------------------------------------------
		//inicjalizacja macierzy rotacji wokół osi z
		Rz.at<double>(0,0)=cos(gamma);
		Rz.at<double>(0,1)=-sin(gamma);
		Rz.at<double>(0,2)=0.0;

		Rz.at<double>(1,0)=sin(gamma);
		Rz.at<double>(1,1)=cos(gamma);
		Rz.at<double>(1,2)=0.0;

		Rz.at<double>(2,0)=0.0;
		Rz.at<double>(2,1)=0.0;
		Rz.at<double>(2,2)=1.0;
		//------------------------------------------

		R=Rz*Ry*Rx; //obliczenie macierzy rotacji całkowitej

		transpose(obiekt,obiekt); //transpozycja macierzy obiektu przed rotacją

		obiekt=R*obiekt; // rotacja wierzchołków obiektu

		transpose(obiekt,obiekt); //transpozycja macierzy obiektu po rotacji

		return obiekt;
	}

	void sklejObrazy(vector<Mat>& obrazy,Mat& obraz_zbiorczy,int ilosc) //łączenie obrazów poszczególnych ścian w obraz całościowy
	{
		int i,j,k,z;
		int maxx,maxy; //wymiary obrazka
		Scalar px; //kolor danego piksela

		obraz_zbiorczy=obrazy[0].clone(); //inicjalizacja obrazu zbiorczego kopią pierwszego z obrazów składowych
		maxx=obraz_zbiorczy.cols; //pobiera szerokość obrazu
		maxy=obraz_zbiorczy.rows; //pobiera wysokość obrazu

		//kopiowanie wartości pikseli z poszczególnych obrazów do obrazu zbiorczego, jeżeli kolor danego piksela nie jest czarny
		for(z=1;z<ilosc;z++)
			for(j=0;j<maxx;j++)
				for(i=0;i<maxy;i++)
					for(k=0;k<3;k++) //obraz trójkanałowy
					{
						px=obrazy[z].at<Vec3b>(i,j); //pobieram wartość piksela
						if(px.val[k] > 0)
							obraz_zbiorczy.at<Vec3b>(i,j).val[k]=obrazy[z].at<Vec3b>(i,j).val[k]; //wstawienie wartości piksela dla danego kanału
					}
		//----------------------------------------------------------------------------------------------------------------------
	}


	Mat renderujPerspektywe(Szescian& obiekt,Szescian& obiekt_oryg) //tworzy obraz sześcianu z przekszałconymi przez perspektywę bitmapami
	{
		int i;
		vector<Mat> obrazy; //wektor zawierający macierze obrazów poszczególnych ścian sześcianu po przekształceniu perspektywicznym
		Mat obraz_zbiorczy; //obraz końcowy
		Mat trans; //macierz transformacji
		Mat temp; //tymczasowa macierz przechowująca obraz wynikowy przekształcenia perspektywicznego danej ściany

		Mat im[6]; //sześć bitmap dla każdej ze ścian sześcianu
		for(i=0;i<6;i++)
			im[i]=obiekt.get(i).getTekstura(); //przypisuję tekstury ścian

		Point2f pIm[4]; //cztery punkty uzyskane z projekcji
		Point2f pW[4]; //cztery odpowiedniki punktów dla świata

		int j,ilosc_obrazow=0;
		for(j=0;j<6;j++)
		{
			if(obiekt.jestWidoczna[j]) //bierzemy pod uwagę jedynie ściany widoczne
			{
				Vec4i kol=obiekt.get(j).get();

				//przypisanie punktów sześcianu po projekcji z przesunięciem do początku układu współrzędnych i z przeskalowaniem
			    pIm[0]=Point2f((float)(obiekt.w.at<double>(kol[0],0)+1)*im[0].cols/2,(float)(obiekt.w.at<double>(kol[0],1)+1)*im[0].cols/2);
			    pIm[1]=Point2f((float)(obiekt.w.at<double>(kol[1],0)+1)*im[0].cols/2,(float)(obiekt.w.at<double>(kol[1],1)+1)*im[0].cols/2);
			    pIm[2]=Point2f((float)(obiekt.w.at<double>(kol[2],0)+1)*im[0].cols/2,(float)(obiekt.w.at<double>(kol[2],1)+1)*im[0].cols/2);
			    pIm[3]=Point2f((float)(obiekt.w.at<double>(kol[3],0)+1)*im[0].cols/2,(float)(obiekt.w.at<double>(kol[3],1)+1)*im[0].cols/2);  
			    //---------------------------------------------------------------------------------------------------------------
			    //zabezpieczenie przed wystąpieniem czterech punktów na jednej linii
			    for(i=0;i<4;i++)
				    if(pIm[i].x==pIm[(i+1)%4].x || pIm[i].y==pIm[(i+1)%4].y) {
				    	pIm[(i+1)%4].x+=(float)0.1;
				    	pIm[(i+1)%4].y+=(float)0.1;
				    }
			    //------------------------------------------------------------------

				//przypisanie punktów początkowej pozycji sześcianu w przestrzeni 3D z przesunięciem do początku układu współrzędnych i z przeskalowaniem
			    pW[0]=Point2f((float)(obiekt_oryg.w.at<double>(0,0)+1)*(im[0].cols)/2,(float)(obiekt_oryg.w.at<double>(0,1)+1)*(im[0].cols)/2);
			    pW[1]=Point2f((float)(obiekt_oryg.w.at<double>(1,0)+1)*(im[0].cols)/2,(float)(obiekt_oryg.w.at<double>(1,1)+1)*(im[0].cols)/2);
			    pW[2]=Point2f((float)(obiekt_oryg.w.at<double>(2,0)+1)*(im[0].cols)/2,(float)(obiekt_oryg.w.at<double>(2,1)+1)*(im[0].cols)/2);
			    pW[3]=Point2f((float)(obiekt_oryg.w.at<double>(3,0)+1)*(im[0].cols)/2,(float)(obiekt_oryg.w.at<double>(3,1)+1)*(im[0].cols)/2);
			    //---------------------------------------------------------------------------------------------------------------------------------------

			    Mat pImMat=(Mat_<float>(4,2) << //przerobienie wektorów na macierz dla funkcji findHomography()
			    	pIm[0].x,pIm[0].y,
			    	pIm[1].x,pIm[1].y,
			    	pIm[2].x,pIm[2].y,
			    	pIm[3].x,pIm[3].y);
			    Mat pWMat=(Mat_<float>(4,2) << //przerobienie wektorów na macierz dla funkcji findHomography()
			    	pW[0].x,pW[0].y,
			    	pW[1].x,pW[1].y,
			    	pW[2].x,pW[2].y,
			    	pW[3].x,pW[3].y);

			    trans=findHomography(pImMat,pWMat); //obliczenie macierzy transformacji homograficznej

			    Mat tmat=(Mat_<double>(3,3) << //macierz translacji dla transformacji homograficznej 
			    	1,0,im[j].cols/2,
			    	0,1,im[j].cols/2,
			    	0,0,1);

			    trans=trans.inv(); // inwersja macierzy transformacji homograficznej
			    trans=tmat*trans;  // translacja w dół i w lewo odpowiednio o połowę wysokości i szerokości bitmapy

			    warpPerspective(im[j],temp,trans,Size(2*im[j].cols,2*im[j].rows)); // mapowanie perspektywiczne z poszerzeniem pola widzenia

			    obrazy.push_back(temp.clone()); //dodanie wynikowego obrazu do wektora obrazów

				ilosc_obrazow++; //zwiększenie licznika wygenerowanych obrazów
			}
		}

		sklejObrazy(obrazy,obraz_zbiorczy,ilosc_obrazow); //łączenie obrazów poszczególnych ścian w obraz całościowy

		return obraz_zbiorczy;
	}

	void okreslWektNorm(Szescian& sz) //funkcja oblicza wektor normalny danej ściany i ustawia wartość flagi widoczności ściany
	{
		int i;
		Vec3f wektor1,wektor2;
		Vec4i scn;
		Point3f p,p1,p2,p_srodek;

		for(i=0;i<6;i++)
		{
			scn=sz.get(i).get(); //pobiera kolejność wierzchołków ściany o numerze i

			//znalezienie punktu leżącego na środku ściany
			p1=Point3f(sz.w.row(scn[0])); 
			p2=Point3f(sz.w.row(scn[2]));
			p_srodek=Point3f((p1.x+p2.x)/(float)2.0,(p1.y+p2.y)/(float)2.0,(p1.z+p2.z)/(float)2.0);

			//znalezienie dwóch koplanarnych wektorów
			p=p_srodek;
			p1=Point3f(sz.w.row(scn[1])); 
			p2=Point3f(sz.w.row(scn[0]));

			wektor1=Vec3f(p1.x-p.x,p1.y-p.y,p1.z-p.z);
			wektor2=Vec3f(p2.x-p.x,p2.y-p.y,p2.z-p.z);
			//---------------------------------------

			//znalezienie wektora normalnego---------
			Mat wektory=(Mat_<float>(2,3) <<
				wektor1[0],wektor1[1],wektor1[2],
				wektor2[0],wektor2[1],wektor2[2]);

			Vec3f normal=wektory.row(0).cross(wektory.row(1)); //obliczenie wektora normalnego jako cross-produkt dwóch koplanarnych wektorów
			//---------------------------------------

			//ustawienie flag widoczności dla danej ściany
			sz.jestWidoczna[i]=(normal[2]>-0.4)?0:1; //warunek sprawdzony eksperymentalnie (1 ozn. ściana jest widoczna, 0 - niewidoczna)
			//---------------------------------------
		}
	}

	Mat updateView(Szescian& sz,double alfa, double beta, double gamma) //aktualizacja obrazu (obrót kostki o kąt alfa, beta lub gamma i utworzenie obrazu; kąty wyrażone w stopniach)
	{
		Szescian sz_oryg; //oryginał sześcianu jako punkt odniesienia do obliczeń (ten nie będzie obracany)
		Mat im; //obraz wynikowy

		alfa=alfa*CV_PI/180.0; //w radianach
		beta=beta*CV_PI/180.0; //w radianach
		gamma=gamma*CV_PI/180.0; //w radianach

		//rotacja-----------------------------
		sz.w=obrot(sz_oryg.w,alfa,beta,gamma);
		//------------------------------------

		okreslWektNorm(sz); //znajduje wektory normalne poszczególnych ścian i odpowiednio ustawia flagi ścian

		//projekcja-------------------------------------------------------------------------------------------
		double f=3; // ogniskowa kamery

		Mat camM=(Mat_<double>(3,3) << //macierz kamery
			f,0.0,0,
			0.0,f,0,
			0.0,0.0,1.0);

		Vec3d rvec(0.0,0.0,0.0),tvec(0.0,0.0,5.0); //wektor rotacji (zerowy, tylko na potrzeby funkcji projectPoints()) oraz wektor translacji, taki by współrzędne punktów na osi z były dodatnie
		double aspectRatio=0.0; //wartość zerowa oznacza domyślną wartość aspektu

		projectPoints(sz.w,rvec,tvec,camM,noArray(),sz.w,noArray(),aspectRatio); //projekcja wierzchołków sześcianu na płaszczyznę 2D
		//-----------------------------------------------------------------------------------------------------

		return renderujPerspektywe(sz,sz_oryg); //tworzy obraz sześcianu z przekszałconymi przez perspektywę bitmapami
	}

	void o_programie(void) //wyświetlenie informacji o programie
	{
		cout << "\n";
		cout << "/===================================================================/\n";
		cout << "Program obracajacy kostke (szescian) przy uzyciu biblioteki OpenCV  /\n";
		cout << "Autor: Bartosz Pacula, wersja 1.0\t\t\t\t    /\n";
		cout << "Styczen 2019r.\t\t\t\t\t    OpenCV ver.3.2.0/\n";
		cout << "/===================================================================/\n";
		cout << "Sterowanie:\t\t\t\t\t\t\t    /\n";
		cout << "klawisze \"a\" i \"z\" - obrot wzgledem osi x\t\t\t    /\n";
		cout << "klawisze \"s\" i \"x\" - obrot wzgledem osi y\t\t\t    /\n";
		cout << "klawisze \"d\" i \"c\" - obrot wzgledem osi z\t\t\t    /\n";
		cout << "klawisz \"q\" - wyjscie z programu\t\t\t\t    /\n";
		cout << "(Sterowanie tylko przy aktywnym oknie kostki)\t\t\t    /\n";
		cout << "/===================================================================/\n";
	}
} //koniec definicji przestrzeni nazw: kostka
