namespace TexView
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this._chkAutoUpdate = new System.Windows.Forms.CheckBox();
            this._pbTexture = new System.Windows.Forms.PictureBox();
            this._cboTextureId = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this._nudUpdateFrequency = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this._txtHostName = new System.Windows.Forms.TextBox();
            this._txtPort = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this._btnConnect = new System.Windows.Forms.Button();
            this._txtOutput = new System.Windows.Forms.TextBox();
            this._btnTestSend = new System.Windows.Forms.Button();
            this._txtTestText = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this._mnuViewConsole = new System.Windows.Forms.ToolStripMenuItem();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this._nudRandomByteCount = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.button4 = new System.Windows.Forms.Button();
            this._tmrUpdate = new System.Windows.Forms.Timer(this.components);
            this.button5 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this._pbTexture)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._nudUpdateFrequency)).BeginInit();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._nudRandomByteCount)).BeginInit();
            this.SuspendLayout();
            // 
            // _chkAutoUpdate
            // 
            this._chkAutoUpdate.AutoSize = true;
            this._chkAutoUpdate.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._chkAutoUpdate.Location = new System.Drawing.Point(232, 41);
            this._chkAutoUpdate.Name = "_chkAutoUpdate";
            this._chkAutoUpdate.Size = new System.Drawing.Size(119, 24);
            this._chkAutoUpdate.TabIndex = 0;
            this._chkAutoUpdate.Text = "Auto Update";
            this._chkAutoUpdate.UseVisualStyleBackColor = true;
            // 
            // _pbTexture
            // 
            this._pbTexture.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._pbTexture.Location = new System.Drawing.Point(16, 396);
            this._pbTexture.Name = "_pbTexture";
            this._pbTexture.Size = new System.Drawing.Size(501, 43);
            this._pbTexture.TabIndex = 1;
            this._pbTexture.TabStop = false;
            // 
            // _cboTextureId
            // 
            this._cboTextureId.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._cboTextureId.FormattingEnabled = true;
            this._cboTextureId.Location = new System.Drawing.Point(96, 39);
            this._cboTextureId.Name = "_cboTextureId";
            this._cboTextureId.Size = new System.Drawing.Size(121, 28);
            this._cboTextureId.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(83, 20);
            this.label1.TabIndex = 3;
            this.label1.Text = "Texture ID";
            // 
            // _nudUpdateFrequency
            // 
            this._nudUpdateFrequency.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._nudUpdateFrequency.Location = new System.Drawing.Point(357, 39);
            this._nudUpdateFrequency.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this._nudUpdateFrequency.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this._nudUpdateFrequency.Name = "_nudUpdateFrequency";
            this._nudUpdateFrequency.Size = new System.Drawing.Size(63, 26);
            this._nudUpdateFrequency.TabIndex = 4;
            this._nudUpdateFrequency.Value = new decimal(new int[] {
            500,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(426, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(30, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "ms";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(524, 28);
            this.menuStrip1.TabIndex = 6;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(46, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 24);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // _txtHostName
            // 
            this._txtHostName.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._txtHostName.Location = new System.Drawing.Point(56, 74);
            this._txtHostName.Name = "_txtHostName";
            this._txtHostName.Size = new System.Drawing.Size(257, 26);
            this._txtHostName.TabIndex = 8;
            this._txtHostName.Text = "localhost";
            // 
            // _txtPort
            // 
            this._txtPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._txtPort.Location = new System.Drawing.Point(363, 74);
            this._txtPort.Name = "_txtPort";
            this._txtPort.Size = new System.Drawing.Size(71, 26);
            this._txtPort.TabIndex = 9;
            this._txtPort.Text = "46298";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(319, 77);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(38, 20);
            this.label4.TabIndex = 10;
            this.label4.Text = "Port";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(7, 77);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(43, 20);
            this.label5.TabIndex = 10;
            this.label5.Text = "Host";
            // 
            // _btnConnect
            // 
            this._btnConnect.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._btnConnect.Location = new System.Drawing.Point(56, 106);
            this._btnConnect.Name = "_btnConnect";
            this._btnConnect.Size = new System.Drawing.Size(112, 31);
            this._btnConnect.TabIndex = 11;
            this._btnConnect.Text = "Connect";
            this._btnConnect.UseVisualStyleBackColor = true;
            this._btnConnect.Click += new System.EventHandler(this._btnConnect_Click);
            // 
            // _txtOutput
            // 
            this._txtOutput.Dock = System.Windows.Forms.DockStyle.Bottom;
            this._txtOutput.Location = new System.Drawing.Point(0, 445);
            this._txtOutput.Multiline = true;
            this._txtOutput.Name = "_txtOutput";
            this._txtOutput.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this._txtOutput.Size = new System.Drawing.Size(524, 136);
            this._txtOutput.TabIndex = 12;
            // 
            // _btnTestSend
            // 
            this._btnTestSend.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._btnTestSend.Location = new System.Drawing.Point(277, 106);
            this._btnTestSend.Name = "_btnTestSend";
            this._btnTestSend.Size = new System.Drawing.Size(74, 31);
            this._btnTestSend.TabIndex = 11;
            this._btnTestSend.Text = "Send";
            this._btnTestSend.UseVisualStyleBackColor = true;
            this._btnTestSend.Click += new System.EventHandler(this._btnTestSend_Click);
            // 
            // _txtTestText
            // 
            this._txtTestText.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._txtTestText.Location = new System.Drawing.Point(363, 108);
            this._txtTestText.Name = "_txtTestText";
            this._txtTestText.Size = new System.Drawing.Size(149, 26);
            this._txtTestText.TabIndex = 13;
            this._txtTestText.Text = "*FromClient*";
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(300, 149);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(199, 31);
            this.button1.TabIndex = 11;
            this.button1.Text = "Send 10000 Same";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this._mnuViewConsole});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(55, 24);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // _mnuViewConsole
            // 
            this._mnuViewConsole.Name = "_mnuViewConsole";
            this._mnuViewConsole.Size = new System.Drawing.Size(152, 24);
            this._mnuViewConsole.Text = "Console";
            this._mnuViewConsole.Click += new System.EventHandler(this._mnuViewConsole_Click);
            // 
            // button2
            // 
            this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button2.Location = new System.Drawing.Point(277, 186);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(222, 31);
            this.button2.TabIndex = 11;
            this.button2.Text = "Send 10000 Random Data";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button3.Location = new System.Drawing.Point(56, 223);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(218, 31);
            this.button3.TabIndex = 11;
            this.button3.Text = "Send Specific Data Size";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // _nudRandomByteCount
            // 
            this._nudRandomByteCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._nudRandomByteCount.Location = new System.Drawing.Point(280, 226);
            this._nudRandomByteCount.Maximum = new decimal(new int[] {
            1000000000,
            0,
            0,
            0});
            this._nudRandomByteCount.Name = "_nudRandomByteCount";
            this._nudRandomByteCount.Size = new System.Drawing.Size(120, 26);
            this._nudRandomByteCount.TabIndex = 14;
            this._nudRandomByteCount.Value = new decimal(new int[] {
            512000,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(406, 228);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 20);
            this.label3.TabIndex = 3;
            this.label3.Text = "Bytes";
            // 
            // button4
            // 
            this.button4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button4.Location = new System.Drawing.Point(300, 271);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(199, 31);
            this.button4.TabIndex = 11;
            this.button4.Text = "Send 10000 Ordered";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // _tmrUpdate
            // 
            this._tmrUpdate.Enabled = true;
            this._tmrUpdate.Interval = 1;
            this._tmrUpdate.Tick += new System.EventHandler(this._tmrUpdate_Tick);
            // 
            // button5
            // 
            this.button5.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button5.Location = new System.Drawing.Point(42, 149);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(126, 68);
            this.button5.TabIndex = 11;
            this.button5.Text = "Request Shadowmap Texture Data";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(524, 581);
            this.Controls.Add(this._nudRandomByteCount);
            this.Controls.Add(this._txtTestText);
            this.Controls.Add(this._txtOutput);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.button5);
            this.Controls.Add(this._btnTestSend);
            this.Controls.Add(this._btnConnect);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this._txtPort);
            this.Controls.Add(this._txtHostName);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._nudUpdateFrequency);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this._cboTextureId);
            this.Controls.Add(this._pbTexture);
            this.Controls.Add(this._chkAutoUpdate);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "Vault/Bro OpenGL 4.3 Texture Viewer";
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this._pbTexture)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._nudUpdateFrequency)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._nudRandomByteCount)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox _chkAutoUpdate;
        private System.Windows.Forms.PictureBox _pbTexture;
        private System.Windows.Forms.ComboBox _cboTextureId;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown _nudUpdateFrequency;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.TextBox _txtHostName;
        private System.Windows.Forms.TextBox _txtPort;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button _btnConnect;
        private System.Windows.Forms.TextBox _txtOutput;
        private System.Windows.Forms.Button _btnTestSend;
        private System.Windows.Forms.TextBox _txtTestText;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem _mnuViewConsole;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        public System.Windows.Forms.NumericUpDown _nudRandomByteCount;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Timer _tmrUpdate;
        private System.Windows.Forms.Button button5;
    }
}

