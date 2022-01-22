#pragma once
#include "src/Server.h";
#include "src/Main.h";
#include <chrono>;
#include <string>




namespace Il2Dials
{
	using namespace System::Diagnostics;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Resources;
	


	
	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{


	private:
		int portNumber = 11200;
		//int clients = 0;
		int serverProgressMessage = 0;
		int gameWorkerProgressReport;		
		bool skipCheckBoxEvent = false;
		bool waitingToRestart = false;
		int errorMessageLimit = 9;
		System::String^ currentIcon = "";
		//loading at launch
		
		//System::Drawing::Icon^ cadetBlueStarIcon;
		System::Drawing::Icon^ redStarIcon;
		System::Drawing::Icon^ yellowStarIcon;
		System::Drawing::Icon^ blueStarIcon;
		System::Drawing::Icon^ greenStarIcon;
		System::Drawing::Icon^ darkRedStarIcon;
		
		

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

		System::Drawing::Color darkRed = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(163)), static_cast<System::Int32>(static_cast<System::Byte>(7
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(13)));

		System::Drawing::Color green = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(18)), static_cast<System::Int32>(static_cast<System::Byte>(136
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(6)));

	private: System::Windows::Forms::CheckBox^ LocalClientCheckBox;
	private: System::ComponentModel::BackgroundWorker^ restartWorker;
	private: System::Windows::Forms::RichTextBox^ versionBox;
	private: System::Windows::Forms::RichTextBox^ DebugTextBox2;
	private: System::ComponentModel::BackgroundWorker^ serverWorkerTCP;






		System::Drawing::Color blue = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(18)), static_cast<System::Int32>(static_cast<System::Byte>(60
			)),
			static_cast<System::Int32>(static_cast<System::Byte>(157)));

	public:

		Form1(void)
		{
			// Grab the assembly this is being called from		
			//auto resourceAssembly = Reflection::Assembly::GetExecutingAssembly();
			//use to get working directory
			auto iconsDirectory = AppDomain::CurrentDomain->BaseDirectory + "Icons\\";

			//asign icons manually, c++/clr is missing support for resource file usage
			//cadetBlueStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "cadetBlueStar.ico");
			redStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "redStar.ico");
			yellowStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "yellowStar.ico");
			blueStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "blueStar.ico");
			darkRedStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "darkRedStar.ico");
			greenStarIcon = gcnew System::Drawing::Icon(iconsDirectory + "greenStar.ico");
			//set to red to start - before initialising components
			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "darkRedStar.ico");

			InitializeComponent();


			
			//config			
			char* appDataPath = getenv("LOCALAPPDATA");
			std::string pathString(appDataPath);
			pathString.append("\\Il-2 Dials\\IL2DialsConfig.txt");			
			String^ pathSystemString = gcnew String(pathString.c_str());
			
			if (System::IO::File::Exists(pathSystemString))
			{
				System::IO::StreamReader^ reader = gcnew System::IO::StreamReader(pathSystemString);

				//port number
				portTextBox->Text = reader->ReadLine();

				//ip checkbox
				String^ ipCheckedString = reader->ReadLine();
				bool ipChecked = false;
				if (ipCheckedString == "True")
				{
					ipChecked = true;
					//event fires whether we change checkbox programtically or using the form - set bool to skip this when we read from the file on startup
					skipCheckBoxEvent = true;
				}
				
				LocalClientCheckBox->Checked = ipChecked;

				reader->Close();


			}
			else
			{
				portTextBox->Text = portNumber.ToString();
			}

			//send to server
			int value = Convert::ToInt32(portTextBox->Text);
			SetPortNumber(value);

			this->notifyIcon1->Icon = darkRedStarIcon;// cadetBlueStarIcon;
			this->pictureBox1->ImageLocation = iconsDirectory + "setting-gears3.png";
			//set colour of font star

			starLabel->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(163)), static_cast<System::Int32>(static_cast<System::Byte>(7
				)),
				static_cast<System::Int32>(static_cast<System::Byte>(13)));

			//start our backgroudn workers (asyncs)
			serverWorkerUDP->RunWorkerAsync();
			serverWorkerTCP->RunWorkerAsync();
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
private: System::ComponentModel::BackgroundWorker^ serverWorkerUDP;

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
			this->serverWorkerUDP = (gcnew System::ComponentModel::BackgroundWorker());
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
			this->LocalClientCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->restartWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->versionBox = (gcnew System::Windows::Forms::RichTextBox());
			this->DebugTextBox2 = (gcnew System::Windows::Forms::RichTextBox());
			this->serverWorkerTCP = (gcnew System::ComponentModel::BackgroundWorker());
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
			// serverWorkerUDP
			// 
			this->serverWorkerUDP->WorkerReportsProgress = true;
			this->serverWorkerUDP->WorkerSupportsCancellation = true;
			this->serverWorkerUDP->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::ServerWorkerUDP_DoWork);
			this->serverWorkerUDP->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::ServerWorkerUDP_ProgressChanged);
			// 
			// notifyIcon1
			// 
			this->notifyIcon1->ContextMenuStrip = this->contextMenuStrip1;
			this->notifyIcon1->Text = L"IL-2 Dials Server";
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
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26));
			this->label1->ForeColor = System::Drawing::Color::CadetBlue;
			this->label1->Location = System::Drawing::Point(10, 57);
			this->label1->Margin = System::Windows::Forms::Padding(0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(42, 42);
			this->label1->TabIndex = 3;
			this->label1->Text = L"IL";
			// 
			// starLabel
			// 
			this->starLabel->AutoSize = true;
			this->starLabel->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->starLabel->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(163)), static_cast<System::Int32>(static_cast<System::Byte>(7
				)),static_cast<System::Int32>(static_cast<System::Byte>(13))); //dark red
			this->starLabel->Location = System::Drawing::Point(38, 57);
			this->starLabel->Margin = System::Windows::Forms::Padding(0);
			this->starLabel->Name = L"starLabel";
			this->starLabel->Size = System::Drawing::Size(43, 42);
			this->starLabel->TabIndex = 4;
			this->starLabel->Text = L"★";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Century Gothic", 26));
			this->label3->ForeColor = System::Drawing::Color::CadetBlue;
			this->label3->Location = System::Drawing::Point(70, 57);
			this->label3->Margin = System::Windows::Forms::Padding(0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(139, 42);
			this->label3->TabIndex = 4;
			this->label3->Text = L"2 DIALS";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Century Gothic", 24));
			this->label4->ForeColor = System::Drawing::Color::CadetBlue;
			this->label4->Location = System::Drawing::Point(171, 9);
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
			this->portTextBox->Font = (gcnew System::Drawing::Font(L"Century Gothic", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->portTextBox->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->portTextBox->Location = System::Drawing::Point(90, 290);
			this->portTextBox->Name = L"portTextBox";
			this->portTextBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->portTextBox->Size = System::Drawing::Size(106, 20);
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
			this->portLabel->Font = (gcnew System::Drawing::Font(L"Century Gothic", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->portLabel->ForeColor = System::Drawing::Color::CadetBlue;
			this->portLabel->Location = System::Drawing::Point(47, 290);
			this->portLabel->Margin = System::Windows::Forms::Padding(0);
			this->portLabel->Name = L"portLabel";
			this->portLabel->Size = System::Drawing::Size(45, 21);
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
			this->DebugTextBox->Location = System::Drawing::Point(29, 188);
			this->DebugTextBox->Name = L"DebugTextBox";
			this->DebugTextBox->ReadOnly = true;
			this->DebugTextBox->Size = System::Drawing::Size(167, 36);
			this->DebugTextBox->TabIndex = 9;
			this->DebugTextBox->Text = L"Initialising...";
			this->DebugTextBox->Visible = false;
			// 
			// LocalClientCheckBox
			// 
			this->LocalClientCheckBox->AutoSize = true;
			this->LocalClientCheckBox->CheckAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->LocalClientCheckBox->ForeColor = System::Drawing::Color::CadetBlue;
			this->LocalClientCheckBox->Location = System::Drawing::Point(48, 254);
			this->LocalClientCheckBox->Name = L"LocalClientCheckBox";
			this->LocalClientCheckBox->Size = System::Drawing::Size(148, 17);
			this->LocalClientCheckBox->TabIndex = 10;
			this->LocalClientCheckBox->Text = L"Local Client (IP 127.0.0.1)";
			this->LocalClientCheckBox->UseVisualStyleBackColor = true;
			this->LocalClientCheckBox->Visible = false;
			this->LocalClientCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::LocalClientCheckBox_CheckedChanged);
			// 
			// restartWorker
			// 
			this->restartWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::RestartWorker_DoWork);
			// 
			// versionBox
			// 
			this->versionBox->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->versionBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->versionBox->Cursor = System::Windows::Forms::Cursors::Default;
			this->versionBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->versionBox->ForeColor = System::Drawing::Color::CadetBlue;
			this->versionBox->Location = System::Drawing::Point(160, 102);
			this->versionBox->Name = L"versionBox";
			this->versionBox->ReadOnly = true;
			this->versionBox->Size = System::Drawing::Size(49, 21);
			this->versionBox->TabIndex = 11;
			this->versionBox->Text = L"v0.4.01";
			this->versionBox->Visible = false;
			// 
			// DebugTextBox2
			// 
			this->DebugTextBox2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->DebugTextBox2->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->DebugTextBox2->Cursor = System::Windows::Forms::Cursors::Default;
			this->DebugTextBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DebugTextBox2->ForeColor = System::Drawing::Color::CadetBlue;
			this->DebugTextBox2->Location = System::Drawing::Point(26, 147);
			this->DebugTextBox2->Name = L"DebugTextBox2";
			this->DebugTextBox2->ReadOnly = true;
			this->DebugTextBox2->Size = System::Drawing::Size(167, 36);
			this->DebugTextBox2->TabIndex = 12;
			this->DebugTextBox2->Text = L"";
			this->DebugTextBox2->Visible = false;
			// 
			// serverWorkerTCP
			// 
			this->serverWorkerTCP->WorkerReportsProgress = true;
			this->serverWorkerTCP->WorkerSupportsCancellation = true;
			this->serverWorkerTCP->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::ServerWorkerTCP_DoWork);
			this->serverWorkerTCP->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Form1::ServerWorkerTCP_ProgressChanged);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(21)), static_cast<System::Int32>(static_cast<System::Byte>(24)),
				static_cast<System::Int32>(static_cast<System::Byte>(24)));
			this->ClientSize = System::Drawing::Size(218, 331);
			this->Controls->Add(this->DebugTextBox2);
			this->Controls->Add(this->versionBox);
			this->Controls->Add(this->LocalClientCheckBox);
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
			this->Text = L"Il-2 Dials";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
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
	private: System::Void ServerWorkerUDP_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{

		//Debug::WriteLine("Server Worker");
		//start server, passing the worker so it can report back using it
		bool localIP = LocalClientCheckBox->Checked;
		StartServerUDP(serverWorkerUDP, localIP);

	}

	private: System::Void ServerWorkerUDP_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
	{
		//clients = e->ProgressPercentage; //enable for socket error tracking
		serverProgressMessage = e->ProgressPercentage;
		UpdateReports();
	}


	private: System::Void ServerWorkerTCP_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		//unused, enable to track client numbers

		//Debug::WriteLine("Server Worker TCP");
		//start server, passing the worker so it can report back using it
		//bool localIP = LocalClientCheckBox->Checked;
		//StartServerTCP(serverWorkerTCP, localIP);

	}

	private: System::Void ServerWorkerTCP_ProgressChanged(System::Object^ sender, System::ComponentModel::ProgressChangedEventArgs^ e)
	{
		//clients = e->ProgressPercentage; //udp now doesn't track clients

		UpdateReports();
	}

	private: System::Void GameWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) 
	{
		Injector(gameWorker);
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

		if (waitingToRestart)
		{
			//wait for safe restart, wait for code cave background worker
			DebugTextBox->Text = "Waiting to safely restart...";

			return;
		}

		
		//look for turn needle scanner (only in test builds)
		//bool needleTest = true;
		//only returned offsets pass over 100
		if (gameWorkerProgressReport > 100)
		{
			if (gameWorkerProgressReport <= errorMessageLimit)
				DebugTextBox2->Text = "Scanning for needle offset...";
			else
			{
				//convert to hex for print
				char c = System::Convert::ToChar(gameWorkerProgressReport);
				System::String^ sysString = System::Convert::ToString(c);

				DebugTextBox2->Text = System::Convert::ToString("offset found at = " + gameWorkerProgressReport);
			}
				
			return;
		}


	//for icon loading
	//use to get working directory
	auto iconsDirectory = AppDomain::CurrentDomain->BaseDirectory + "Icons\\";

	
	if (serverProgressMessage == -10)
	{
		//can't rely on icon type check - control ourself with string compare
		if (currentIcon != "yellow")
		{	
			//yellow font star			
			starLabel->ForeColor = yellow;

			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "yellowStar.ico");;
			this->notifyIcon1->Icon = yellowStarIcon;

			DebugTextBox->Text = "Could not find local IP - 10";
			currentIcon = "yellow";
		}
		return;
	}
	if (serverProgressMessage == -20)
	{
		if (currentIcon != "yellow")
		{
			//yellow font star			
			starLabel->ForeColor = yellow;

			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "yellowStar.ico");;
			this->notifyIcon1->Icon = yellowStarIcon;

			DebugTextBox->Text = "Socket address error - 20";
			currentIcon = "yellow";
		}
		return;
	}

	if (gameWorkerProgressReport == 0)
	{
		//waiting for game
		if (currentIcon != "darkRed")
		{
			starLabel->ForeColor = darkRed;

			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "darkRedStar.ico");
			this->notifyIcon1->Icon = darkRedStarIcon;

			DebugTextBox->Text = "Waiting for game...";
			currentIcon = "darkRed";
		}
		return;
	}
	
	if (gameWorkerProgressReport < errorMessageLimit)
	{
		if (currentIcon != "yellow")
		{		
			//yellow font star			
			starLabel->ForeColor = yellow;

			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "yellowStar.ico");;
			this->notifyIcon1->Icon = yellowStarIcon;

			DebugTextBox->Text = "Can not read data from game";
			currentIcon = "yellow";
		}
		return;
	}

	if (gameWorkerProgressReport == errorMessageLimit)
	{
		if (currentIcon != "green")
		{
			//game patched and udp sending
			
			starLabel->ForeColor = green;

			this->Icon = gcnew System::Drawing::Icon(iconsDirectory + "greenStar.ico");;
			this->notifyIcon1->Icon = greenStarIcon;

			DebugTextBox->Text = "Reading data, sending UDP";
			currentIcon = "green";
		}

		return;
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
		LocalClientCheckBox->Visible = !LocalClientCheckBox->Visible;
		versionBox->Visible = !versionBox->Visible;
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

			//write file and end process
			WriteConfigFile();

		}

	}


	private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) 
	{
		//get rid of the icon in the systray
		NotifyIcon^ thisIcon = (NotifyIcon^)notifyIcon1;
		thisIcon->Visible = false;
	}
		private: System::Void contextMenuStrip2_Opening(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
		}
		private: System::Void pictureBox2_Click_1(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void LocalClientCheckBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			//write file and restart process
			//skip bool will be set on first read of file
			if (!skipCheckBoxEvent)
			{
				WriteConfigFile();				
			}
			else
			{
				//reset skip so if box i changed we will reset and save
				skipCheckBoxEvent = false;
			}
		}

	private: System::Void WriteConfigFile()
	{

		//Port Number
		int value = Convert::ToInt32(portTextBox->Text);
		//tell server 
		SetPortNumber(value);


		//remmember in file format
		char* appDataPath = getenv("LOCALAPPDATA");
		std::string pathString(appDataPath);
		pathString.append("\\Il-2 Dials\\IL2DialsConfig.txt");
		String^ pathSystemString = gcnew String(pathString.c_str());
		System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(pathSystemString); //open the file for writing.

		std:String^ portNumber = portTextBox->Text->ToString();
		writer->WriteLine(portNumber); //write the current date to the file. change this with your date or something.
			

		//Local Ip Checkbox
		bool localIPChecked = LocalClientCheckBox->Checked;
		String^ ipString = localIPChecked.ToString();

		//write new line with bool value
		writer->WriteLine(ipString);

		//close the file again.
		writer->Close(); 
		
		// dispose/delete it from the memory.
		delete writer;


		//get rid of the icon in the systray
		NotifyIcon^ thisIcon = (NotifyIcon^)notifyIcon1;
		thisIcon->Visible = false;


		//now we set flag 
		waitingToRestart = true;

		//and text
		//wait for safe restart, wait for code cave background worker
		DebugTextBox->Text = "Waiting to safely restart..";

		//and run worker if not already run - This way user can change their mind ( a restart will still occur though)
		if(!restartWorker->IsBusy)
			restartWorker->RunWorkerAsync();
		

	}


	private: System::Void RestartApp()
	{
		Application::Restart();
		Environment::Exit(0);
	}
	private: System::Void RestartWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		//check for safe restart
		while (gameWorkerProgressReport != errorMessageLimit && gameWorkerProgressReport != 0)
		{
			System::Threading::Thread::Sleep(10);
		}

		//if we get here
		RestartApp();

	}

};

	

}
