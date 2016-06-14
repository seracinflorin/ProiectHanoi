#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"


HINSTANCE _hInstance;
int _nCmdShow;
MSG msg;
HWND _mainwnd;
HWND _nr_turnuri;
//Pentru afisarea turnurilor
//-------------------------
//Datele despre fiecare turn h,l x,y
struct date_turn
{
	HWND turn;
	int x;
	int lung;
};

struct Stativ
{
	date_turn corp[9];		//Turnurile ce se afla pe acest stativ
	int nr_turnuri = 0;		//Numarul de turnuri de pe stativ
};
Stativ stative[4];			//Cele 3 stative
Stativ capie_stative[4];	//Copii pentru initializari;
Stativ ccapie_stative[4];	//Copie pentru ce e pe ecran
int mutari_hanoi;			//Cate mutari am executat

//Variabile Globale Hanoi
int nr_turnuri=0;
int n;					//Nuamr de turnuri
char mutari[1500][3];	//Mutarile care trebuiesc efectuate
int nr_mutari = 0;		//Numarul de mutari pe care le avem de facut
void alg_hanoi();


//Procedura ferestrei principale
void Muta_stativ(char surs, char dest)
{
	//Vedem care e sursa si destinatia sa vedem cum facem mutarile
	int ssurs; int ddest;
	if (surs == 'a')	ssurs = 1;
	if (surs == 'b')	ssurs = 2;
	if (surs == 'c')	ssurs = 3;

	if (dest == 'a')	ddest = 1;
	if (dest == 'b')	ddest = 2;
	if (dest == 'c')	ddest = 3;


	stative[ssurs].nr_turnuri--;
	stative[ddest].nr_turnuri++;

	//stative[ddest].corp[stative[ddest].nr_turnuri].turn = stative[ssurs].corp[stative[ssurs].nr_turnuri + 1].turn;

	stative[ddest].corp[stative[ddest].nr_turnuri].x = stative[ssurs].corp[stative[ssurs].nr_turnuri + 1].x;
	stative[ddest].corp[stative[ddest].nr_turnuri].lung = stative[ssurs].corp[stative[ssurs].nr_turnuri + 1].lung;
}
bool CHECK(char *sir){
	if (strlen(sir) == 0)
		return false;
	for (int i = 0; i < strlen(sir); i++)
	{
		if (!isdigit(sir[i]))
			return false;
	}
	return true;
}
void ButoaneFunctionale(HWND hwnd)
{
	CreateWindow("BUTTON", "Next Step", WS_BORDER | WS_CHILD | WS_VISIBLE, 60, 540, 200, 30, hwnd, (HMENU)IDC_BUTTON2, _hInstance, NULL);
	CreateWindow("BUTTON", "Reset", WS_BORDER | WS_CHILD | WS_VISIBLE, 310, 540, 200, 30, hwnd, (HMENU)IDC_BUTTON3, _hInstance, NULL);
	CreateWindow("BUTTON", "Finish", WS_BORDER | WS_CHILD | WS_VISIBLE, 560, 540, 200, 30, hwnd, (HMENU)IDC_BUTTON4, _hInstance, NULL);
	CreateWindow("BUTTON", "HELP", WS_BORDER | WS_CHILD | WS_VISIBLE, 810, 540, 200, 30, hwnd, (HMENU)IDC_BUTTON5, _hInstance, NULL);

	CreateWindow("static", "Introduceti numarul de turnuri <1-8> :", WS_BORDER | WS_CHILD | WS_VISIBLE, 0, 20, 297, 25, hwnd, (HMENU)1, _hInstance, NULL);
	_nr_turnuri = CreateWindow("edit", "", WS_BORDER | WS_CHILD | WS_VISIBLE, 300, 20, 20, 25, hwnd, (HMENU)1, _hInstance, NULL);
	CreateWindow("BUTTON", "Start", WS_BORDER | WS_CHILD | WS_VISIBLE, 340, 18, 200, 30, hwnd, (HMENU)IDC_BUTTON1, _hInstance, NULL);
}
void PaintBCK(HWND hwnd,HDC hdc)
{
	//forma culoare si stilul de desenare
	HPEN hPenOld;
	HPEN hLinePen;
	COLORREF qLineColor;
	HBRUSH hBrush, holdBrush;

	//incarcam imaginea de pe fundal
	HBITMAP bmpExercising;// handle pentru imagine
	HDC MemDCExercising;

	bmpExercising = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	MemDCExercising = CreateCompatibleDC(hdc);
	SelectObject(MemDCExercising, bmpExercising);
	BitBlt(hdc, 0, 0, 1220, 730, MemDCExercising, 0, 0, SRCCOPY);//afisam imaginea dupa ce am incarcat-o
	//stergem handle-urile pentru a goli memorie
	DeleteDC(MemDCExercising);
	DeleteObject(bmpExercising);

	//-----------------------------
	//Desenam suportii pentru Hanoi
	qLineColor = RGB(11, 11, 11);
	hLinePen = CreatePen(PS_SOLID, 12, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);

	//desenam primul stativ
	MoveToEx(hdc, 50, 500, NULL);
	LineTo(hdc, 300, 500);
	MoveToEx(hdc, 175, 100, NULL);
	LineTo(hdc, 175, 500);

	//al doilea
	MoveToEx(hdc, 370, 500, NULL);
	LineTo(hdc, 620, 500);
	MoveToEx(hdc, 495, 100, NULL);
	LineTo(hdc, 495, 500);

	//al treilea
	MoveToEx(hdc, 690, 500, NULL);
	LineTo(hdc, 940, 500);
	MoveToEx(hdc, 815, 100, NULL);
	LineTo(hdc, 815, 500);
}
void Background_Hanoi(HWND hwnd)
{
	//procedura pentru a desena in fereastra 
	HDC hdc;
	PAINTSTRUCT Ps;
	hdc = BeginPaint(hwnd, &Ps);
	HPEN hPenOld;
	HPEN hLinePen;
	COLORREF qLineColor;
	HBRUSH hBrush, holdBrush;
	
	PaintBCK(hwnd, hdc);

	//Afisam cele n turnuri introduse
	if (nr_turnuri && mutari_hanoi == 0)
	{
		for (int i = 1; i <= nr_turnuri; i++)
		{
			stative[1].corp[i].turn = CreateWindowW(L"HanoiClass", NULL, WS_CHILD | WS_VISIBLE, 55 + 14 * (i - 1), 100 + 50 * (8 - i), 240 - 28 * (i - 1), 50, _mainwnd, NULL, _hInstance, NULL);
			stative[1].corp[i].x = 55 + 14 * (i - 1);
			stative[1].corp[i].lung = 240 - 28 * (i - 1);
			//Facem o copie
			capie_stative[1].corp[i].turn = stative[1].corp[i].turn;
			capie_stative[1].corp[i].x = stative[1].corp[i].x;
			capie_stative[1].corp[i].lung = stative[1].corp[i].lung;
			//Facem o cope la ferestre
			ccapie_stative[1].corp[i].turn = stative[1].corp[i].turn;
			ccapie_stative[1].corp[i].x = stative[1].corp[i].x;
			ccapie_stative[1].corp[i].lung = stative[1].corp[i].lung;
		}
		stative[1].nr_turnuri = nr_turnuri;
		capie_stative[1].nr_turnuri = nr_turnuri;
		ccapie_stative[1].nr_turnuri = nr_turnuri;
	}

	if (mutari_hanoi)
	{
		//Distrugem vechile ferestre
		for (int i = 1; i <= nr_turnuri; i++)
			DestroyWindow(ccapie_stative[1].corp[i].turn);

		for (int i = 1; i <= nr_turnuri; i++)
			DestroyWindow(ccapie_stative[2].corp[i].turn);

		for (int i = 1; i <= nr_turnuri; i++)
			DestroyWindow(ccapie_stative[3].corp[i].turn);


		//Facem mutarile pentru nmarul de pasi
		for (int i = 0; i < mutari_hanoi; i++)
			Muta_stativ(mutari[i][0],mutari[i][1]);

		for (int i = 1; i <= stative[1].nr_turnuri; i++)
			stative[1].corp[i].turn = CreateWindowW(L"HanoiClass", NULL, WS_CHILD | WS_VISIBLE, stative[1].corp[i].x, 100 + 50 * (8 - i), stative[1].corp[i].lung, 50, _mainwnd, NULL, _hInstance, NULL);

		for (int i = 1; i <= stative[2].nr_turnuri; i++)
			stative[2].corp[i].turn = CreateWindowW(L"HanoiClass", NULL, WS_CHILD | WS_VISIBLE, 320 + stative[2].corp[i].x, 100 + 50 * (8 - i), stative[2].corp[i].lung, 50, _mainwnd, NULL, _hInstance, NULL);

		for (int i = 1; i <= stative[3].nr_turnuri; i++)
			stative[3].corp[i].turn = CreateWindowW(L"HanoiClass", NULL, WS_CHILD | WS_VISIBLE, 640 + stative[3].corp[i].x, 100 + 50 * (8 - i), stative[3].corp[i].lung, 50, _mainwnd, NULL, _hInstance, NULL);

		//Facem copie la noile ferestre
		for (int i = 1; i <= 3; i++)
			ccapie_stative[i] = stative[i];

		PaintBCK(hwnd, hdc);
	}

	EndPaint(hwnd, &Ps);
}
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//pictam tot ce se afla in fereastra vezi sa te joci cu comentarii
	case WM_PAINT:
		Background_Hanoi(hWnd);
		break;
		//pentru butoane
	case WM_CREATE:
		ButoaneFunctionale(hWnd);
		break;
		//
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDC_BUTTON1://Butonul START
		{
			char buff[4];
			for (int i = 0; i <= 3; i++)
				buff[i] = '\0';

			//luam  mesajul din nr_turnuri si il folosim
			SendMessage(_nr_turnuri, WM_GETTEXT, sizeof(buff) / sizeof(char), reinterpret_cast<LPARAM>(buff));
			//stergi fereastra si o creezi din nou sa ramana interfata la fel
			DestroyWindow(_nr_turnuri);
			_nr_turnuri = CreateWindow("edit", "", WS_BORDER | WS_CHILD | WS_VISIBLE, 300, 20, 20, 25, hWnd, (HMENU)1, _hInstance, NULL);

			if (CHECK(buff) && atoi(buff) <= 8 && atoi(buff) >=1)
			{
				nr_turnuri = atoi(buff);
				n = nr_turnuri;
				alg_hanoi();
				InvalidateRect(hWnd, NULL, TRUE);
			}
			else
			{
				MessageBoxW(hWnd, L"Valuare incorecta!", L"Error", MB_ICONERROR);
			}
		}
			break;

		case IDC_BUTTON2: //Butonul Next
			for (int i = 1; i <= 3; i++)
				stative[i] = capie_stative[i];
			if (mutari_hanoi < nr_mutari)
			{
				mutari_hanoi++;
				InvalidateRect(hWnd, NULL, TRUE);
			}

			break;
		case IDC_BUTTON4: //Buton Finis
			for (int i = 1; i <= 3; i++)
				stative[i] = capie_stative[i];
			//Numar de mutari ia numarul maxim de mutari si le vom afisa astfel in pozitia finala
			mutari_hanoi = nr_mutari;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDC_BUTTON3: //Buton Resetare
		{
			for (int i = 1; i <= nr_turnuri; i++)
				DestroyWindow(ccapie_stative[1].corp[i].turn);

			for (int i = 1; i <= nr_turnuri; i++)
				DestroyWindow(ccapie_stative[2].corp[i].turn);

			for (int i = 1; i <= nr_turnuri; i++)
				DestroyWindow(ccapie_stative[3].corp[i].turn);

			nr_turnuri = 0;
			mutari_hanoi = 0;
			nr_mutari = 0;
			InvalidateRect(hWnd, NULL, TRUE);
		}
			break;

		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Procedura TurnHanoi
LRESULT CALLBACK HanoiProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT Ps;
			hdc = BeginPaint(hwnd, &Ps);

			HPEN hPenOld;
			HPEN hLinePen;
			COLORREF qLineColor;
			HBRUSH hBrush, holdBrush;

			RECT rect;
			GetWindowRect(hwnd, &rect);
			//creeam un bloc cu margine neagra si interior negru
			qLineColor = RGB(1, 1, 1);
			hLinePen = CreatePen(PS_SOLID, 6, qLineColor);
			hPenOld = (HPEN)SelectObject(hdc, hLinePen);
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			holdBrush = (HBRUSH)SelectObject(hdc, hBrush);

			RoundRect(hdc, 3, 3, rect.right-rect.left-3, rect.bottom-rect.top-3, 5, 5);

		}
		break;

	case WM_LBUTTONUP:
		// daca dai click se aude beep
		MessageBeep(MB_OK);
		break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//INT MAIN
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	_nCmdShow = nShowCmd;
	_hInstance = hInst;

	//Blocurile Hanoi || clasa windows pentru blocurile hanoi
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 0, 0));

	WNDCLASSW rwc = { 0 };

	rwc.lpszClassName = L"HanoiClass";
	rwc.hbrBackground = hbrush;
	rwc.lpfnWndProc = HanoiProc;
	rwc.hCursor = LoadCursor(0, IDC_ARROW);

	//inregistram tot in memorie... functie windows
	RegisterClassW(&rwc);
	//-----------------

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = _hInstance;
	wClass.lpfnWndProc = (WNDPROC)WinProc;
	wClass.lpszClassName = "MAin";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx(&wClass)){
		int nResult = GetLastError();
		//mesaj de eroare daca nu s-a inregistrat fereastra principala
		MessageBox(NULL, "Nu sa putut inregistra fereastra!", "Error", MB_ICONERROR);
	}

	//handle la fereastra adica data care inregisteaza fereastra (un fel de pointer)
	HWND hWnd = CreateWindowEx(NULL, "MAin", "Turnurile din Hanoi", WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 1042, 647, NULL, NULL, hInst, NULL);
	_mainwnd = hWnd;
	if (!hWnd){
		int nResult = GetLastError();
		MessageBox(NULL, "Nu sa creat fereastra MAin", "Error", MB_ICONERROR);
	}
	//afisam ferastra dupa ce am inregistrat-o si creat-o
	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


//Algoritm Hanoi
char a, b, c;

void han(int n, char a, char b, char c)
{
	if (n == 1)
	{
		//cout << a << b << endl;
		mutari[nr_mutari][0] = a;
		mutari[nr_mutari][1] = b;
		mutari[nr_mutari][2] = '\0';
		nr_mutari++;
	}
	else
	{
		han(n - 1, a, c, b);
		mutari[nr_mutari][0] = a;
		mutari[nr_mutari][1] = b;
		mutari[nr_mutari][2] = '\0';
		nr_mutari++;
		han(n - 1, c, b, a);
	}
}
void alg_hanoi()
{
	a = 'a'; b = 'b'; c = 'c';
	han(n, a, b, c);
}
