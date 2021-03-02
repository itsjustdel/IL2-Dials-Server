#pragma once
#include "src/Server.h";
#include "src/Main.h";
#include <chrono>;
namespace ServerApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;


	
	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{


	private:
		int portNumber = 11200;
		int clients = 0;
		int gameWorkerProgressReport;
		

		//colors
		System::Drawing::Color cadetBlue = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(95)), static_cast<System::Int32>(static_cast<System::Byte>(158)),
			static_cast<System::Int32>(static_cast<System::Byte>(160)));
		System::Drawing::Color red = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(235)), static_cast<System::Int32>(static_cast<System::Byte>(2)),
			static_cast<System::Int32>(static_cast<System::Byte>(2)));
		System::Drawing::Color yellow = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(235)), static_cast<System::Int32>(static_cast<System::Byte>(195
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(2)));

		System::Drawing::Color peach = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(198
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(151)));

		System::Drawing::Color orange = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(235)), static_cast<System::Int32>(static_cast<System::Byte>(108
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(2)));

		System::Drawing::Color blue = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(18)), static_cast<System::Int32>(static_cast<System::Byte>(60
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(157)));

	public:

		Form1(void)
		{
			InitializeComponent();

			//config
			//(File.Exists(curFile)
			if (System::IO::File::Exists("config.txt"))
			{
				System::IO::StreamReader^ reader = gcnew System::IO::StreamReader("config.txt");

				//check text is legal

				portTextBox->Text = reader->ReadToEnd();

				reader->Close();


			}
			else
			{
				portTextBox->Text = portNumber.ToString();
			}



			//main icon - have to do here or winforms constanstly overwrites it
			this->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\cadetBlueStar.ico");
			//apply icon after initialised - have to do here or winforms constanstly overwrites it
			this->notifyIcon1->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\cadetBlueStar.ico");

			starLabel->ForeColor = cadetBlue;//

			//start our backgroudn workers (asyncs)
			serverWorker->RunWorkerAsync();
			gameWorker->RunWorkerAsync();
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::ComponentModel::BackgroundWorker^ gameWorker;
	private: System::ComponentModel::BackgroundWorker^ serverWorker;
	private: System::Windows::Forms::NotifyIcon^ notifyIcon1;
	private: System::Windows::Forms::ContextMenuStrip^ contextMenuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^ toolStripMenuItem1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ starLabel;

	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::ToolStripMenuItem^ toolStripMenuItem2;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::TextBox^ portTextBox;

	private: System::Windows::Forms::Label^ portLabel;
	private: System::Windows::Forms::RichTextBox^ DebugTextBox;
	private: System::Windows::Forms::Timer^ timer1;










	private: System::ComponentModel::IContainer^ components;


	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->gameWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->serverWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->notifyIcon1 = (gcnew System::Windows::Forms::NotifyIcon(this->components));
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->starLabel = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->portTextBox = (gcnew System::Windows::Forms::TextBox());
			this->portLabel = (gcnew System::Windows::Forms::Label());
			this->DebugTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->contextMenuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// gameWorker
			// 
			this->gameWorker->WorkerReportsProgress = true;
			this->gameWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::GameWorker_DoWork);
			this->gameWorker->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::GameWorker_ProgressChanged);
			// 
			// serverWorker
			// 
			this->serverWorker->WorkerReportsProgress = true;
			this->serverWorker->WorkerSupportsCancellation = true;
			this->serverWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::ServerWorker_DoWork);
			this->serverWorker->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::ServerWorker_ProgressChanged);
			// 
			// notifyIcon1
			// 
			this->notifyIcon1->ContextMenuStrip = this->contextMenuStrip1;
			this->notifyIcon1->Text = L"notifyIcon1";
			this->notifyIcon1->Visible = true;
			this->notifyIcon1->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::notifyIcon1_MouseDoubleClick);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->toolStripMenuItem2,
					this->toolStripMenuItem1
			});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(114, 48);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(113, 22);
			this->toolStripMenuItem2->Text = L"Restore";
			this->toolStripMenuItem2->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItem2_Click);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(113, 22);
			this->toolStripMenuItem1->Text = L"Exit";
			this->toolStripMenuItem1->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItem1_Click);
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26));
			this->label1->ForeColor = System::Drawing::Color::CadetBlue;
			this->label1->Location = System::Drawing::Point(21, 49);
			this->label1->Margin = System::Windows::Forms::Padding(0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(42, 42);
			this->label1->TabIndex = 3;
			this->label1->Text = L"IL";
			// 
			// starLabel
			// 
			this->starLabel->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->starLabel->AutoSize = true;
			this->starLabel->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->starLabel->ForeColor = System::Drawing::Color::CadetBlue;
			this->starLabel->Location = System::Drawing::Point(49, 49);
			this->starLabel->Margin = System::Windows::Forms::Padding(0);
			this->starLabel->Name = L"starLabel";
			this->starLabel->Size = System::Drawing::Size(43, 42);
			this->starLabel->TabIndex = 4;
			this->starLabel->Text = L"★";
			// 
			// label3
			// 
			this->label3->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26));
			this->label3->ForeColor = System::Drawing::Color::CadetBlue;
			this->label3->Location = System::Drawing::Point(81, 49);
			this->label3->Margin = System::Windows::Forms::Padding(0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(139, 42);
			this->label3->TabIndex = 4;
			this->label3->Text = L"2 DIALS";
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Century Gothic", 24));
			this->label4->ForeColor = System::Drawing::Color::CadetBlue;
			this->label4->Location = System::Drawing::Point(203, 0);
			this->label4->Margin = System::Windows::Forms::Padding(0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(38, 39);
			this->label4->TabIndex = 5;
			this->label4->Text = L"⇲";
			this->label4->TextAlign = System::Drawing::ContentAlignment::TopRight;
			this->label4->Click += gcnew System::EventHandler(this, &Form1::label4_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->pictureBox1->ImageLocation = L"setting-gears3.png";
			this->pictureBox1->Location = System::Drawing::Point(12, 286);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(32, 32);
			this->pictureBox1->TabIndex = 6;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Click += gcnew System::EventHandler(this, &Form1::pictureBox1_Click_1);
			// 
			// portTextBox
			// 
			this->portTextBox->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->portTextBox->BackColor = System::Drawing::Color::CadetBlue;
			this->portTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->portTextBox->Cursor = System::Windows::Forms::Cursors::IBeam;
			this->portTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->portTextBox->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->portTextBox->Location = System::Drawing::Point(101, 292);
			this->portTextBox->Name = L"portTextBox";
			this->portTextBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->portTextBox->Size = System::Drawing::Size(119, 19);
			this->portTextBox->TabIndex = 7;
			this->portTextBox->Text = L"11200";
			this->portTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->portTextBox->Visible = false;
			this->portTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::CheckTextInput);
			this->portTextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::OnKeyDownHandler);
			// 
			// portLabel
			// 
			this->portLabel->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->portLabel->AutoSize = true;
			this->portLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->portLabel->ForeColor = System::Drawing::Color::CadetBlue;
			this->portLabel->Location = System::Drawing::Point(53, 291);
			this->portLabel->Margin = System::Windows::Forms::Padding(0);
			this->portLabel->Name = L"portLabel";
			this->portLabel->Size = System::Drawing::Size(42, 20);
			this->portLabel->TabIndex = 8;
			this->portLabel->Text = L"Port:";
			this->portLabel->Visible = false;
			// 
			// DebugTextBox
			// 
			this->DebugTextBox->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->DebugTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->DebugTextBox->Cursor = System::Windows::Forms::Cursors::Default;
			this->DebugTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DebugTextBox->ForeColor = System::Drawing::Color::CadetBlue;
			this->DebugTextBox->Location = System::Drawing::Point(28, 244);
			this->DebugTextBox->Name = L"DebugTextBox";
			this->DebugTextBox->ReadOnly = true;
			this->DebugTextBox->Size = System::Drawing::Size(181, 36);
			this->DebugTextBox->TabIndex = 9;
			this->DebugTextBox->Text = L"bla bla bla ...";
			this->DebugTextBox->Visible = false;
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->ClientSize = System::Drawing::Size(241, 331);
			this->ControlBox = false;
			this->Controls->Add(this->DebugTextBox);
			this->Controls->Add(this->portLabel);
			this->Controls->Add(this->portTextBox);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->starLabel);
			this->Controls->Add(this->label1);
			this->Cursor = System::Windows::Forms::Cursors::Default;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Form1";
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form1_MouseDown);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form1_MouseMove);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form1_MouseUp);
			this->contextMenuStrip1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


		//server worker
	private: System::Void ServerWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		//start server, passing the worker so it can report back using it
		StartServer(serverWorker);
	}

	private: System::Void ServerWorker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
	{
		clients = e->ProgressPercentage;

		UpdateReports();
	}

	private: System::Void GameWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) 
	{
		ReadData(gameWorker);
	}

	private: System::Void GameWorker_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
	{
		gameWorkerProgressReport = e->ProgressPercentage;

		//0 is no process found
		//anything in between,something has failed/or scanning
		//20 is everything ok
		UpdateReports();

	}
private: void UpdateReports()
{

	if (clients > 0)
	{
		if (gameWorkerProgressReport == 0)
		{
			starLabel->ForeColor = blue;
			this->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\blueStar.ico");
			this->notifyIcon1->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\blueStar.ico");

			DebugTextBox->Text = "Waiting for game, client connected...";
		}

		if (gameWorkerProgressReport == 20)
		{
			//all go, set star to red
			this->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\redStar.ico");
			this->notifyIcon1->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\redStar.ico");
			starLabel->ForeColor = red;

			DebugTextBox->Text = "Reading data, client connected";
		}
	}
	else if (clients == 0)
	{
		if (gameWorkerProgressReport == 0)
		{
			//default cadet blue
			this->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\cadetBlueStar.ico");
			this->notifyIcon1->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\cadetBlueStar.ico");
			starLabel->ForeColor = cadetBlue;

			DebugTextBox->Text = "Waiting for game, waiting for client...";
		}

		if (gameWorkerProgressReport == 20)
		{
			//yellow
			this->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\yellowStar.ico");
			this->notifyIcon1->Icon = gcnew System::Drawing::Icon("C:\\Users\\itsju\\Documents\\Visual Studio Projects\\Il2ServerLateNight\\Il2ServerAppLateNight\\Il2ServerAppLateNight\\Icons\\yellowStar.ico");
			starLabel->ForeColor = yellow;

			DebugTextBox->Text = "Reading data, waiting for client...";
		}
	}
}
	//move borderless window 
	private: bool mouseDown;
	private: Point lastLocation;
	private: static System::Windows::Forms::Timer^ myTimer = gcnew System::Windows::Forms::Timer;

	private: System::Void Form1_MouseDown(System::Object^ sender, MouseEventArgs^ e)
	{
		mouseDown = true;
		lastLocation = e->Location;
	}

	private: System::Void Form1_MouseMove(System::Object^ sender, MouseEventArgs^ e)
	{
		if (mouseDown)
		{
			this->Location = System::Drawing::Point(this->Location.X - lastLocation.X + e->X, this->Location.Y - lastLocation.Y + e->Y);

			this->Update();
		}
	}

	private: System::Void Form1_MouseUp(System::Object^ sender, MouseEventArgs^ e)
	{
		mouseDown = false;
	}
	private: System::Void toolStripMenuItem1_Click(System::Object^ sender, System::EventArgs^ e)
	{
		//systray menu
		Application::Exit();
		//exit
	}

	private: System::Void notifyIcon1_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		//systray
		//restore
		this->Show();
	}

	private: System::Void label4_Click(System::Object^ sender, System::EventArgs^ e)
	{
		// main form
		//minimise
		this->Hide();
	}

	private: System::Void toolStripMenuItem2_Click(System::Object^ sender, System::EventArgs^ e)
	{
		//systray
			//restore
		this->Show();
	}


	private: System::Void label5_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void pictureBox1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label6_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void pictureBox2_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void pictureBox1_Click_1(System::Object^ sender, System::EventArgs^ e)
	{
		//gear click, open panel
		//toggle label etc
		portLabel->Visible = !portLabel->Visible;
		portTextBox->Visible = !portTextBox->Visible;
		DebugTextBox->Visible = !DebugTextBox->Visible;
	}


	private: System::Void CheckTextInput(System::Object^ sender, System::EventArgs^ e)
	{
		//set tiemr to 3 seconds
		timer1->Interval = 1000;

		//check if valid port number


		timer1->Start();
	}

	private: void OnKeyDownHandler(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
	{

		if (e->KeyCode == Keys::Enter)
		{
			//stop form making noise
			e->SuppressKeyPress = true;

			//remmember in file format
			System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter("config.txt"); //open the file for writing.
		std:String^ portNumber = portTextBox->Text->ToString();
			writer->Write(portNumber); //write the current date to the file. change this with your date or something.
			writer->Close(); //remember to close the file again.
			delete writer;// writer->Dispose(); //remember to dispose it from the memory.

			Application::Restart();
			Environment::Exit(0);

		}

	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e)
	{
		return;
		//fires after interval has passed
		//stop that timer
		timer1->Stop();

		//restart app on port change, blocking call in async worker complicates things, this is simpler	
		System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter("config.txt"); //open the file for writing.
	std:String^ portNumber = portTextBox->Text->ToString();
		writer->Write(portNumber); //write the current date to the file. change this with your date or something.
		writer->Close(); //remember to close the file again.
		delete writer;// writer->Dispose(); //remember to dispose it from the memory.

		Application::Restart();
		Environment::Exit(0);
	}


	};

	

}
