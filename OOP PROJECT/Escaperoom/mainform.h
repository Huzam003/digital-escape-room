// MainForm.h
#pragma once

// Native headers
#include "GameManager.h"
#include "User.h"

// Required for string marshalling
#include <msclr/marshal_cppstd.h>

// C++ standard includes (GameManager.h already pulled in <vector>, but no harm)
#include <vector>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;               // for IContainer
using namespace System::Collections::Generic;       // for List<String^>
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace msclr::interop;

namespace EscapeRoomGUI {

    public ref class MainForm : public System::Windows::Forms::Form
    {
    public:
        MainForm(void)
        {
            InitializeComponent();
            InitializeLoginScreen();
        }

    protected:
        ~MainForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        // Native game manager & user
        GameManager* nativeGame;    // pointer to our existing GameManager
        User* nativeUser;           // pointer to our existing User

        // Track puzzle index (nativeUser->getProgress())
        int currentPuzzleIndex;

        // Store inventory items (session only)
        List<String^>^ inventoryList;

        // Keep the vector of Puzzle* so we can query them
        const std::vector<Puzzle*>* puzzles; // pointer to GameManager::getPuzzles()

        // ========== WinForms controls ==========

        // Panels
        Panel^ panelLogin;        // contains username/password + buttons
        Panel^ panelGame;         // contains puzzle display, answer box, submit, inventory

        // Login controls
        Label^ lblUsername;
        TextBox^ txtUsername;
        Label^ lblPassword;
        TextBox^ txtPassword;
        Button^ btnLogin;
        Button^ btnSignup;
        Label^ lblLoginMessage; // for “invalid credentials” etc.

        // Puzzle controls (visible once logged in)
        Label^ lblWelcome;      // “Welcome, <username>!”
        Label^ lblProgress;     // “You have completed X out of Y puzzles.”
        Label^ lblPuzzleText;   // The current puzzle’s question
        TextBox^ txtAnswer;      // Where user types their answer
        Button^ btnSubmitAnswer;// Press to submit
        ListBox^ listInventory;  // Lists collected items
        Label^ lblInventory;
        Button^ btnReset;       // “Play Again” after finishing all puzzles
        Button^ btnExit;        // “Exit Game” button

        IContainer^ components;

#pragma region "Windows Form Designer generated code"
        void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();

            // Basic form properties
            this->Text = L"Digital Escape Room";
            this->ClientSize = System::Drawing::Size(800, 600);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->FormBorderStyle = FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
        }
#pragma endregion

        // -------- Initialize either Login screen or Game screen --------

        void InitializeLoginScreen()
        {
            // Create native backend objects
            nativeGame = new GameManager();
            nativeUser = new User();
            inventoryList = gcnew List<String^>();

            // ====== LOGIN PANEL ======
            panelLogin = gcnew Panel();
            panelLogin->Size = System::Drawing::Size(800, 600);
            panelLogin->Location = Point(0, 0);

            // Username label + textbox
            lblUsername = gcnew Label();
            lblUsername->Text = L"Username:";
            lblUsername->Location = Point(200, 200);
            lblUsername->AutoSize = true;

            txtUsername = gcnew TextBox();
            txtUsername->Location = Point(300, 200);
            txtUsername->Width = 200;

            // Password label + textbox
            lblPassword = gcnew Label();
            lblPassword->Text = L"Password:";
            lblPassword->Location = Point(200, 240);
            lblPassword->AutoSize = true;

            txtPassword = gcnew TextBox();
            txtPassword->Location = Point(300, 240);
            txtPassword->Width = 200;
            txtPassword->UseSystemPasswordChar = true;

            // Login button
            btnLogin = gcnew Button();
            btnLogin->Text = L"Log In";
            btnLogin->Location = Point(300, 280);
            btnLogin->Click += gcnew EventHandler(this, &MainForm::OnLoginClicked);

            // Signup button
            btnSignup = gcnew Button();
            btnSignup->Text = L"Sign Up";
            btnSignup->Location = Point(400, 280);
            btnSignup->Click += gcnew EventHandler(this, &MainForm::OnSignupClicked);

            // Message label for errors / status
            lblLoginMessage = gcnew Label();
            lblLoginMessage->Text = L"";
            lblLoginMessage->Location = Point(300, 320);
            lblLoginMessage->ForeColor = Color::Red;
            lblLoginMessage->AutoSize = true;

            // Add login controls to panel
            panelLogin->Controls->Add(lblUsername);
            panelLogin->Controls->Add(txtUsername);
            panelLogin->Controls->Add(lblPassword);
            panelLogin->Controls->Add(txtPassword);
            panelLogin->Controls->Add(btnLogin);
            panelLogin->Controls->Add(btnSignup);
            panelLogin->Controls->Add(lblLoginMessage);

            // Add login panel to the form
            this->Controls->Add(panelLogin);

            // Prepare (but don’t show) the game panel yet
            InitializeGameScreen();
        }

        void InitializeGameScreen()
        {
            // ====== GAME PANEL (initially hidden) ======
            panelGame = gcnew Panel();
            panelGame->Size = System::Drawing::Size(800, 600);
            panelGame->Location = Point(0, 0);
            panelGame->Visible = false; // will become true after login

            // “Welcome, <username>!” label
            lblWelcome = gcnew Label();
            lblWelcome->Text = L"Welcome!";
            lblWelcome->Font = gcnew Font(L"Segoe UI", 14, FontStyle::Bold);
            lblWelcome->Location = Point(20, 20);
            lblWelcome->AutoSize = true;

            // Progress label: “You have completed X out of Y puzzles.”
            lblProgress = gcnew Label();
            lblProgress->Text = L"";
            lblProgress->Location = Point(20, 60);
            lblProgress->AutoSize = true;

            // Puzzle text label (multiline area)
            lblPuzzleText = gcnew Label();
            lblPuzzleText->Text = L"";
            lblPuzzleText->Location = Point(20, 100);
            lblPuzzleText->Size = System::Drawing::Size(760, 200);
            lblPuzzleText->Font = gcnew Font(L"Segoe UI", 10);
            lblPuzzleText->AutoSize = false;

            // Answer textbox
            txtAnswer = gcnew TextBox();
            txtAnswer->Location = Point(20, 320);
            txtAnswer->Width = 500;

            // Submit Answer button
            btnSubmitAnswer = gcnew Button();
            btnSubmitAnswer->Text = L"Submit";
            btnSubmitAnswer->Location = Point(540, 318);
            btnSubmitAnswer->Click += gcnew EventHandler(this, &MainForm::OnSubmitAnswerClicked);

            // Inventory label + ListBox
            lblInventory = gcnew Label();
            lblInventory->Text = L"Inventory:";
            lblInventory->Location = Point(20, 360);
            lblInventory->AutoSize = true;

            listInventory = gcnew ListBox();
            listInventory->Location = Point(20, 390);
            listInventory->Size = System::Drawing::Size(300, 150);

            // Reset & Exit buttons (shown only at game over)
            btnReset = gcnew Button();
            btnReset->Text = L"Play Again";
            btnReset->Location = Point(350, 500);
            btnReset->Visible = false;
            btnReset->Click += gcnew EventHandler(this, &MainForm::OnResetClicked);

            btnExit = gcnew Button();
            btnExit->Text = L"Exit Game";
            btnExit->Location = Point(470, 500);
            btnExit->Visible = false;
            btnExit->Click += gcnew EventHandler(this, &MainForm::OnExitClicked);

            // Add all game controls to panelGame
            panelGame->Controls->Add(lblWelcome);
            panelGame->Controls->Add(lblProgress);
            panelGame->Controls->Add(lblPuzzleText);
            panelGame->Controls->Add(txtAnswer);
            panelGame->Controls->Add(btnSubmitAnswer);
            panelGame->Controls->Add(lblInventory);
            panelGame->Controls->Add(listInventory);
            panelGame->Controls->Add(btnReset);
            panelGame->Controls->Add(btnExit);

            // Add game panel to the form (but keep it hidden for now)
            this->Controls->Add(panelGame);
        }

        // ========== Event handlers ==========

        // 1) User clicks “Log In”
        void OnLoginClicked(Object^ sender, EventArgs^ e)
        {
            String^ u = txtUsername->Text->Trim();
            String^ p = txtPassword->Text;

            if (String::IsNullOrWhiteSpace(u) || String::IsNullOrWhiteSpace(p))
            {
                lblLoginMessage->Text = L"Please enter both username and password.";
                return;
            }

            // Marshal managed String^ to std::string
            std::string usernameStd = marshal_as<std::string>(u);
            std::string passwordStd = marshal_as<std::string>(p);

            // Attempt authentication
            if (nativeUser->authenticate(usernameStd, passwordStd))
            {
                lblLoginMessage->ForeColor = Color::Green;
                lblLoginMessage->Text = L"Login successful!";

                // Switch to game panel
                SwitchToGameScreen(u);
            }
            else
            {
                lblLoginMessage->ForeColor = Color::Red;
                lblLoginMessage->Text = L"Invalid credentials. Please try again.";
            }
        }

        // 2) User clicks “Sign Up”
        void OnSignupClicked(Object^ sender, EventArgs^ e)
        {
            String^ u = txtUsername->Text->Trim();
            String^ p = txtPassword->Text;

            if (String::IsNullOrWhiteSpace(u) || String::IsNullOrWhiteSpace(p))
            {
                lblLoginMessage->Text = L"Please enter both username and password.";
                return;
            }

            std::string usernameStd = marshal_as<std::string>(u);
            std::string passwordStd = marshal_as<std::string>(p);

            // Create a new account (console‐style)
            nativeUser->setUsername(usernameStd);
            nativeUser->setPassword(passwordStd);
            nativeUser->setProgress(0);
            nativeUser->saveProgress();

            lblLoginMessage->ForeColor = Color::Green;
            lblLoginMessage->Text = L"Account created. You can now log in.";
        }

        // Switch from login to game, initialize first puzzle
        void SwitchToGameScreen(String^ managedUsername)
        {
            panelLogin->Visible = false;
            panelGame->Visible = true;

            // Update welcome label
            lblWelcome->Text = L"Welcome, " + managedUsername + L"!";

            // Grab native puzzles vector via the public getter
            puzzles = &nativeGame->getPuzzles();

            // Current progress from nativeUser
            currentPuzzleIndex = nativeUser->getProgress();
            inventoryList->Clear();

            LoadCurrentPuzzle();
        }

        // Load and display the puzzle at currentPuzzleIndex
        void LoadCurrentPuzzle()
        {
            int total = static_cast<int>(puzzles->size());
            // Update progress label
            String^ progressText = String::Format(
                L"You have completed {0} out of {1} puzzles.",
                currentPuzzleIndex, total
            );
            lblProgress->Text = progressText;

            if (currentPuzzleIndex >= total)
            {
                // Game over screen
                lblPuzzleText->Text = L"*** Congratulations! You have cleared the entire escape room! ***";
                txtAnswer->Visible = false;
                btnSubmitAnswer->Visible = false;

                // Show final inventory
                lblInventory->Text = L"Your final inventory:";
                listInventory->Items->Clear();
                for each (String ^ item in inventoryList)
                    listInventory->Items->Add(item);

                // Show Reset/Exit
                btnReset->Visible = true;
                btnExit->Visible = true;
                return;
            }

            // Otherwise, display next puzzle
            Puzzle* p = (*puzzles)[currentPuzzleIndex];
            std::string qStd = p->getQuestion();
            String^ qManaged = marshal_as<String^>(qStd);
            lblPuzzleText->Text = qManaged;
            txtAnswer->Text = L"";
        }

        // 3) User clicks “Submit” on an answer
        void OnSubmitAnswerClicked(Object^ sender, EventArgs^ e)
        {
            if (currentPuzzleIndex >= static_cast<int>(puzzles->size()))
                return; // nothing to do

            String^ answerManaged = txtAnswer->Text->Trim();
            if (String::IsNullOrWhiteSpace(answerManaged))
            {
                MessageBox::Show(
                    L"Please type an answer before submitting.",
                    L"Empty Answer",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            // Lowercase the managed string
            String^ answerLower = answerManaged->ToLowerInvariant();
            std::string answerStd = marshal_as<std::string>(answerLower);

            // Check correctness
            Puzzle* p = (*puzzles)[currentPuzzleIndex];
            bool correct = p->checkAnswer(answerStd);

            if (correct)
            {
                MessageBox::Show(
                    L"Correct!",
                    L"Good Job",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                // Award item
                std::string rewardStd = p->getRewardItem();
                String^ rewardManaged = marshal_as<String^>(rewardStd);
                inventoryList->Add(rewardManaged);

                // Update nativeUser progress & save
                nativeUser->setProgress(currentPuzzleIndex + 1);
                nativeUser->saveProgress();

                currentPuzzleIndex++;
                LoadCurrentPuzzle();
            }
            else
            {
                MessageBox::Show(
                    L"Wrong answer. Try again.",
                    L"Incorrect",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        // 4) User clicks “Play Again”
        void OnResetClicked(Object^ sender, EventArgs^ e)
        {
            // Reset nativeUser progress
            nativeUser->setProgress(0);
            nativeUser->saveProgress();

            // Reload puzzles so Room 1 reshuffles
            nativeGame->loadPuzzles();

            currentPuzzleIndex = 0;
            inventoryList->Clear();

            // Hide Reset/Exit, show puzzle controls again
            txtAnswer->Visible = true;
            btnSubmitAnswer->Visible = true;
            btnReset->Visible = false;
            btnExit->Visible = false;

            LoadCurrentPuzzle();
        }

        // 5) User clicks “Exit Game”
        void OnExitClicked(Object^ sender, EventArgs^ e)
        {
            Application::Exit();
        }
    };
}
