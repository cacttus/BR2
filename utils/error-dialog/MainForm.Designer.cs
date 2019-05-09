namespace WindowsErrorDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this._txtErrorDetails = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this._lblErrorDetails = new System.Windows.Forms.LinkLabel();
            this._btnOk = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this._txtLogFilePath = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // _txtErrorDetails
            // 
            this._txtErrorDetails.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtErrorDetails.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._txtErrorDetails.Location = new System.Drawing.Point(12, 158);
            this._txtErrorDetails.Multiline = true;
            this._txtErrorDetails.Name = "_txtErrorDetails";
            this._txtErrorDetails.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this._txtErrorDetails.Size = new System.Drawing.Size(1052, 399);
            this._txtErrorDetails.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Lucida Sans", 36F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(77, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(169, 56);
            this.label1.TabIndex = 1;
            this.label1.Text = "Oops!";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(12, 83);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(324, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "There was an error and the game had to exit.";
            // 
            // _lblErrorDetails
            // 
            this._lblErrorDetails.AutoSize = true;
            this._lblErrorDetails.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._lblErrorDetails.LinkColor = System.Drawing.Color.Gray;
            this._lblErrorDetails.Location = new System.Drawing.Point(9, 139);
            this._lblErrorDetails.Name = "_lblErrorDetails";
            this._lblErrorDetails.Size = new System.Drawing.Size(82, 16);
            this._lblErrorDetails.TabIndex = 3;
            this._lblErrorDetails.TabStop = true;
            this._lblErrorDetails.Text = "Error Details";
            this._lblErrorDetails.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this._lblErrorDetails_LinkClicked);
            // 
            // _btnOk
            // 
            this._btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this._btnOk.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._btnOk.Location = new System.Drawing.Point(989, 595);
            this._btnOk.Name = "_btnOk";
            this._btnOk.Size = new System.Drawing.Size(75, 32);
            this._btnOk.TabIndex = 4;
            this._btnOk.Text = "Close";
            this._btnOk.UseVisualStyleBackColor = true;
            this._btnOk.Click += new System.EventHandler(this._btnOk_Click);
            // 
            // label3
            // 
            this.label3.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(8, 566);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(102, 20);
            this.label3.TabIndex = 5;
            this.label3.Text = "Log File Path";
            // 
            // _txtLogFilePath
            // 
            this._txtLogFilePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtLogFilePath.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._txtLogFilePath.Location = new System.Drawing.Point(120, 563);
            this._txtLogFilePath.Name = "_txtLogFilePath";
            this._txtLogFilePath.Size = new System.Drawing.Size(944, 26);
            this._txtLogFilePath.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(12, 103);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(338, 20);
            this.label4.TabIndex = 2;
            this.label4.Text = "Please report this problem. (Click Close to exit).";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::WindowsErrorDialog.Properties.Resources.Untitled;
            this.pictureBox1.Location = new System.Drawing.Point(13, 9);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(58, 52);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1077, 636);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this._txtLogFilePath);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._btnOk);
            this.Controls.Add(this._lblErrorDetails);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this._txtErrorDetails);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Text = "App Crash";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox _txtErrorDetails;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.LinkLabel _lblErrorDetails;
        private System.Windows.Forms.Button _btnOk;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox _txtLogFilePath;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

