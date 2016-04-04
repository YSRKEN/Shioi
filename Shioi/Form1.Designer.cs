namespace Shioi {
	partial class MainForm {
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージ リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
		protected override void Dispose(bool disposing) {
			if(disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows フォーム デザイナーで生成されたコード

		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent() {
			this.MenuStrip = new System.Windows.Forms.MenuStrip();
			this.FileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.NewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.OpenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.SaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.ExitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.EditToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.CopyMoveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.PasteMoveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.CopyBoardTextoolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.CopyBoardPictureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.CopyBoardandTurnTextAToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.GameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.ForwardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.BackwardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.FirstMoveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.LastMoveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.StartComputingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.StopComputingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.HelpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.AboutAToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.StatusStrip = new System.Windows.Forms.StatusStrip();
			this.LastMoveStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.TurnPlayerStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.StepStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.PictureBox = new System.Windows.Forms.PictureBox();
			this.BackwardButton = new System.Windows.Forms.Button();
			this.ForwardButton = new System.Windows.Forms.Button();
			this.FirstMoveButton = new System.Windows.Forms.Button();
			this.LastMoveButton = new System.Windows.Forms.Button();
			this.StartComputingButton = new System.Windows.Forms.Button();
			this.StopComputingButton = new System.Windows.Forms.Button();
			this.ShowMoveNumberComboBox = new System.Windows.Forms.ComboBox();
			this.ElapsedTimeStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.MenuStrip.SuspendLayout();
			this.StatusStrip.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.PictureBox)).BeginInit();
			this.SuspendLayout();
			// 
			// MenuStrip
			// 
			this.MenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.FileToolStripMenuItem,
            this.EditToolStripMenuItem,
            this.GameToolStripMenuItem,
            this.HelpToolStripMenuItem});
			this.MenuStrip.Location = new System.Drawing.Point(0, 0);
			this.MenuStrip.Name = "MenuStrip";
			this.MenuStrip.Size = new System.Drawing.Size(424, 24);
			this.MenuStrip.TabIndex = 0;
			this.MenuStrip.Text = "MenuStrip";
			// 
			// FileToolStripMenuItem
			// 
			this.FileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.NewToolStripMenuItem,
            this.OpenToolStripMenuItem,
            this.SaveToolStripMenuItem,
            this.toolStripSeparator1,
            this.ExitToolStripMenuItem});
			this.FileToolStripMenuItem.Name = "FileToolStripMenuItem";
			this.FileToolStripMenuItem.Size = new System.Drawing.Size(51, 20);
			this.FileToolStripMenuItem.Text = "File(&F)";
			// 
			// NewToolStripMenuItem
			// 
			this.NewToolStripMenuItem.Name = "NewToolStripMenuItem";
			this.NewToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
			this.NewToolStripMenuItem.Size = new System.Drawing.Size(171, 22);
			this.NewToolStripMenuItem.Text = "New(&N)";
			this.NewToolStripMenuItem.Click += new System.EventHandler(this.NewToolStripMenuItem_Click);
			// 
			// OpenToolStripMenuItem
			// 
			this.OpenToolStripMenuItem.ForeColor = System.Drawing.SystemColors.ControlText;
			this.OpenToolStripMenuItem.Name = "OpenToolStripMenuItem";
			this.OpenToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
			this.OpenToolStripMenuItem.Size = new System.Drawing.Size(171, 22);
			this.OpenToolStripMenuItem.Text = "Open(&O)...";
			this.OpenToolStripMenuItem.Click += new System.EventHandler(this.OpenToolStripMenuItem_Click);
			// 
			// SaveToolStripMenuItem
			// 
			this.SaveToolStripMenuItem.Name = "SaveToolStripMenuItem";
			this.SaveToolStripMenuItem.Size = new System.Drawing.Size(171, 22);
			this.SaveToolStripMenuItem.Text = "Save(&A)...";
			this.SaveToolStripMenuItem.Click += new System.EventHandler(this.SaveToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(168, 6);
			// 
			// ExitToolStripMenuItem
			// 
			this.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem";
			this.ExitToolStripMenuItem.Size = new System.Drawing.Size(171, 22);
			this.ExitToolStripMenuItem.Text = "Exit(&X)";
			this.ExitToolStripMenuItem.Click += new System.EventHandler(this.ExitToolStripMenuItem_Click);
			// 
			// EditToolStripMenuItem
			// 
			this.EditToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CopyMoveToolStripMenuItem,
            this.PasteMoveToolStripMenuItem,
            this.CopyBoardTextoolStripMenuItem,
            this.CopyBoardPictureToolStripMenuItem,
            this.CopyBoardandTurnTextAToolStripMenuItem});
			this.EditToolStripMenuItem.Name = "EditToolStripMenuItem";
			this.EditToolStripMenuItem.Size = new System.Drawing.Size(53, 20);
			this.EditToolStripMenuItem.Text = "Edit(&E)";
			// 
			// CopyMoveToolStripMenuItem
			// 
			this.CopyMoveToolStripMenuItem.Name = "CopyMoveToolStripMenuItem";
			this.CopyMoveToolStripMenuItem.Size = new System.Drawing.Size(264, 22);
			this.CopyMoveToolStripMenuItem.Text = "Copy Move(&C)";
			this.CopyMoveToolStripMenuItem.Click += new System.EventHandler(this.CopyMoveToolStripMenuItem_Click);
			// 
			// PasteMoveToolStripMenuItem
			// 
			this.PasteMoveToolStripMenuItem.Name = "PasteMoveToolStripMenuItem";
			this.PasteMoveToolStripMenuItem.Size = new System.Drawing.Size(264, 22);
			this.PasteMoveToolStripMenuItem.Text = "Paste Move(&P)";
			this.PasteMoveToolStripMenuItem.Click += new System.EventHandler(this.PasteMoveToolStripMenuItem_Click);
			// 
			// CopyBoardTextoolStripMenuItem
			// 
			this.CopyBoardTextoolStripMenuItem.Name = "CopyBoardTextoolStripMenuItem";
			this.CopyBoardTextoolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
			this.CopyBoardTextoolStripMenuItem.Size = new System.Drawing.Size(264, 22);
			this.CopyBoardTextoolStripMenuItem.Text = "Copy Board-Text(&T)";
			this.CopyBoardTextoolStripMenuItem.Click += new System.EventHandler(this.CopyBoardTextToolStripMenuItem_Click);
			// 
			// CopyBoardPictureToolStripMenuItem
			// 
			this.CopyBoardPictureToolStripMenuItem.Name = "CopyBoardPictureToolStripMenuItem";
			this.CopyBoardPictureToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.C)));
			this.CopyBoardPictureToolStripMenuItem.Size = new System.Drawing.Size(264, 22);
			this.CopyBoardPictureToolStripMenuItem.Text = "Copy Board-Picture(&B)";
			this.CopyBoardPictureToolStripMenuItem.Click += new System.EventHandler(this.CopyBoardPictureToolStripMenuItem_Click);
			// 
			// CopyBoardandTurnTextAToolStripMenuItem
			// 
			this.CopyBoardandTurnTextAToolStripMenuItem.Name = "CopyBoardandTurnTextAToolStripMenuItem";
			this.CopyBoardandTurnTextAToolStripMenuItem.Size = new System.Drawing.Size(264, 22);
			this.CopyBoardandTurnTextAToolStripMenuItem.Text = "Copy Board-and-Turn-Text(&A)";
			this.CopyBoardandTurnTextAToolStripMenuItem.Click += new System.EventHandler(this.CopyBoardandTurnTextAToolStripMenuItem_Click);
			// 
			// GameToolStripMenuItem
			// 
			this.GameToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ForwardToolStripMenuItem,
            this.BackwardToolStripMenuItem,
            this.FirstMoveToolStripMenuItem,
            this.LastMoveToolStripMenuItem,
            this.toolStripSeparator2,
            this.StartComputingToolStripMenuItem,
            this.StopComputingToolStripMenuItem});
			this.GameToolStripMenuItem.Name = "GameToolStripMenuItem";
			this.GameToolStripMenuItem.Size = new System.Drawing.Size(65, 20);
			this.GameToolStripMenuItem.Text = "Game(&G)";
			// 
			// ForwardToolStripMenuItem
			// 
			this.ForwardToolStripMenuItem.Name = "ForwardToolStripMenuItem";
			this.ForwardToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.C)));
			this.ForwardToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.ForwardToolStripMenuItem.Text = "-> Forward(&O)";
			this.ForwardToolStripMenuItem.Click += new System.EventHandler(this.ForwardToolStripMenuItem_Click);
			// 
			// BackwardToolStripMenuItem
			// 
			this.BackwardToolStripMenuItem.Name = "BackwardToolStripMenuItem";
			this.BackwardToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.X)));
			this.BackwardToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.BackwardToolStripMenuItem.Text = "<- Backward(&B)";
			this.BackwardToolStripMenuItem.Click += new System.EventHandler(this.BackwardToolStripMenuItem_Click);
			// 
			// FirstMoveToolStripMenuItem
			// 
			this.FirstMoveToolStripMenuItem.Name = "FirstMoveToolStripMenuItem";
			this.FirstMoveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.Z)));
			this.FirstMoveToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.FirstMoveToolStripMenuItem.Text = "|<- First move(&F)";
			this.FirstMoveToolStripMenuItem.Click += new System.EventHandler(this.FirstMoveToolStripMenuItem_Click);
			// 
			// LastMoveToolStripMenuItem
			// 
			this.LastMoveToolStripMenuItem.Name = "LastMoveToolStripMenuItem";
			this.LastMoveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.V)));
			this.LastMoveToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.LastMoveToolStripMenuItem.Text = "->| Last move(&L)";
			this.LastMoveToolStripMenuItem.Click += new System.EventHandler(this.LastMoveToolStripMenuItem_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(220, 6);
			// 
			// StartComputingToolStripMenuItem
			// 
			this.StartComputingToolStripMenuItem.Name = "StartComputingToolStripMenuItem";
			this.StartComputingToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.S)));
			this.StartComputingToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.StartComputingToolStripMenuItem.Text = "|> Start computing(&S)";
			this.StartComputingToolStripMenuItem.Click += new System.EventHandler(this.StartComputingToolStripMenuItem_Click);
			// 
			// StopComputingToolStripMenuItem
			// 
			this.StopComputingToolStripMenuItem.Name = "StopComputingToolStripMenuItem";
			this.StopComputingToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.D)));
			this.StopComputingToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
			this.StopComputingToolStripMenuItem.Text = "|| Stop computing(&T)";
			this.StopComputingToolStripMenuItem.Click += new System.EventHandler(this.StopComputingToolStripMenuItem_Click);
			// 
			// HelpToolStripMenuItem
			// 
			this.HelpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.AboutAToolStripMenuItem});
			this.HelpToolStripMenuItem.Name = "HelpToolStripMenuItem";
			this.HelpToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
			this.HelpToolStripMenuItem.Text = "Help(&H)";
			// 
			// AboutAToolStripMenuItem
			// 
			this.AboutAToolStripMenuItem.Name = "AboutAToolStripMenuItem";
			this.AboutAToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
			this.AboutAToolStripMenuItem.Text = "About(&A)...";
			this.AboutAToolStripMenuItem.Click += new System.EventHandler(this.AboutAToolStripMenuItem_Click);
			// 
			// StatusStrip
			// 
			this.StatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.LastMoveStatusLabel,
            this.TurnPlayerStatusLabel,
            this.StepStatusLabel,
            this.ElapsedTimeStatusLabel});
			this.StatusStrip.Location = new System.Drawing.Point(0, 479);
			this.StatusStrip.Name = "StatusStrip";
			this.StatusStrip.Size = new System.Drawing.Size(424, 22);
			this.StatusStrip.TabIndex = 1;
			this.StatusStrip.Text = "StatusStrip";
			// 
			// LastMoveStatusLabel
			// 
			this.LastMoveStatusLabel.Name = "LastMoveStatusLabel";
			this.LastMoveStatusLabel.Size = new System.Drawing.Size(67, 17);
			this.LastMoveStatusLabel.Text = "LastMove : ";
			// 
			// TurnPlayerStatusLabel
			// 
			this.TurnPlayerStatusLabel.Name = "TurnPlayerStatusLabel";
			this.TurnPlayerStatusLabel.Size = new System.Drawing.Size(39, 17);
			this.TurnPlayerStatusLabel.Text = "Turn : ";
			// 
			// StepStatusLabel
			// 
			this.StepStatusLabel.Name = "StepStatusLabel";
			this.StepStatusLabel.Size = new System.Drawing.Size(36, 17);
			this.StepStatusLabel.Text = "Step :";
			// 
			// PictureBox
			// 
			this.PictureBox.Location = new System.Drawing.Point(12, 63);
			this.PictureBox.Name = "PictureBox";
			this.PictureBox.Size = new System.Drawing.Size(400, 400);
			this.PictureBox.TabIndex = 2;
			this.PictureBox.TabStop = false;
			this.PictureBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PictureBox_MouseClick);
			// 
			// BackwardButton
			// 
			this.BackwardButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.BackwardButton.Location = new System.Drawing.Point(58, 27);
			this.BackwardButton.Name = "BackwardButton";
			this.BackwardButton.Size = new System.Drawing.Size(40, 30);
			this.BackwardButton.TabIndex = 3;
			this.BackwardButton.Text = "<-";
			this.BackwardButton.UseVisualStyleBackColor = true;
			this.BackwardButton.Click += new System.EventHandler(this.BackwardButton_Click);
			// 
			// ForwardButton
			// 
			this.ForwardButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.ForwardButton.Location = new System.Drawing.Point(104, 27);
			this.ForwardButton.Name = "ForwardButton";
			this.ForwardButton.Size = new System.Drawing.Size(40, 30);
			this.ForwardButton.TabIndex = 4;
			this.ForwardButton.Text = "->";
			this.ForwardButton.UseVisualStyleBackColor = true;
			this.ForwardButton.Click += new System.EventHandler(this.ForwardButton_Click);
			// 
			// FirstMoveButton
			// 
			this.FirstMoveButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.FirstMoveButton.Location = new System.Drawing.Point(12, 27);
			this.FirstMoveButton.Name = "FirstMoveButton";
			this.FirstMoveButton.Size = new System.Drawing.Size(40, 30);
			this.FirstMoveButton.TabIndex = 5;
			this.FirstMoveButton.Text = "|<-";
			this.FirstMoveButton.UseVisualStyleBackColor = true;
			this.FirstMoveButton.Click += new System.EventHandler(this.FirstMoveButton_Click);
			// 
			// LastMoveButton
			// 
			this.LastMoveButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.LastMoveButton.Location = new System.Drawing.Point(150, 27);
			this.LastMoveButton.Name = "LastMoveButton";
			this.LastMoveButton.Size = new System.Drawing.Size(40, 30);
			this.LastMoveButton.TabIndex = 6;
			this.LastMoveButton.Text = "->|";
			this.LastMoveButton.UseVisualStyleBackColor = true;
			this.LastMoveButton.Click += new System.EventHandler(this.LastMoveButton_Click);
			// 
			// StartComputingButton
			// 
			this.StartComputingButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.StartComputingButton.Location = new System.Drawing.Point(196, 27);
			this.StartComputingButton.Name = "StartComputingButton";
			this.StartComputingButton.Size = new System.Drawing.Size(40, 30);
			this.StartComputingButton.TabIndex = 7;
			this.StartComputingButton.Text = "|>";
			this.StartComputingButton.UseVisualStyleBackColor = true;
			this.StartComputingButton.Click += new System.EventHandler(this.StartComputingButton_Click);
			// 
			// StopComputingButton
			// 
			this.StopComputingButton.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.StopComputingButton.Location = new System.Drawing.Point(242, 27);
			this.StopComputingButton.Name = "StopComputingButton";
			this.StopComputingButton.Size = new System.Drawing.Size(40, 30);
			this.StopComputingButton.TabIndex = 8;
			this.StopComputingButton.Text = "||";
			this.StopComputingButton.UseVisualStyleBackColor = true;
			this.StopComputingButton.Click += new System.EventHandler(this.StopComputingButton_Click);
			// 
			// ShowMoveNumberComboBox
			// 
			this.ShowMoveNumberComboBox.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.ShowMoveNumberComboBox.FormattingEnabled = true;
			this.ShowMoveNumberComboBox.Items.AddRange(new object[] {
            "-1",
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19"});
			this.ShowMoveNumberComboBox.Location = new System.Drawing.Point(288, 31);
			this.ShowMoveNumberComboBox.Name = "ShowMoveNumberComboBox";
			this.ShowMoveNumberComboBox.Size = new System.Drawing.Size(60, 24);
			this.ShowMoveNumberComboBox.TabIndex = 9;
			this.ShowMoveNumberComboBox.Text = "-1";
			this.ShowMoveNumberComboBox.SelectedIndexChanged += new System.EventHandler(this.ShowMoveNumberComboBox_SelectedIndexChanged);
			// 
			// ElapsedTimeStatusLabel
			// 
			this.ElapsedTimeStatusLabel.Name = "ElapsedTimeStatusLabel";
			this.ElapsedTimeStatusLabel.Size = new System.Drawing.Size(0, 17);
			// 
			// MainForm
			// 
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(424, 501);
			this.Controls.Add(this.ShowMoveNumberComboBox);
			this.Controls.Add(this.StopComputingButton);
			this.Controls.Add(this.StartComputingButton);
			this.Controls.Add(this.LastMoveButton);
			this.Controls.Add(this.FirstMoveButton);
			this.Controls.Add(this.ForwardButton);
			this.Controls.Add(this.BackwardButton);
			this.Controls.Add(this.PictureBox);
			this.Controls.Add(this.StatusStrip);
			this.Controls.Add(this.MenuStrip);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MainMenuStrip = this.MenuStrip;
			this.MaximizeBox = false;
			this.Name = "MainForm";
			this.Text = "Shioi";
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.MainForm_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.MainForm_DragEnter);
			this.MenuStrip.ResumeLayout(false);
			this.MenuStrip.PerformLayout();
			this.StatusStrip.ResumeLayout(false);
			this.StatusStrip.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.PictureBox)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.MenuStrip MenuStrip;
		private System.Windows.Forms.ToolStripMenuItem FileToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem NewToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem OpenToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem SaveToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripMenuItem ExitToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem GameToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem ForwardToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem BackwardToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem FirstMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem LastMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
		private System.Windows.Forms.ToolStripMenuItem StartComputingToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem StopComputingToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem HelpToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem AboutAToolStripMenuItem;
		private System.Windows.Forms.StatusStrip StatusStrip;
		private System.Windows.Forms.PictureBox PictureBox;
		private System.Windows.Forms.ToolStripMenuItem EditToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem CopyMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem PasteMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem CopyBoardTextoolStripMenuItem;
		private System.Windows.Forms.Button BackwardButton;
		private System.Windows.Forms.Button ForwardButton;
		private System.Windows.Forms.Button FirstMoveButton;
		private System.Windows.Forms.Button LastMoveButton;
		private System.Windows.Forms.Button StartComputingButton;
		private System.Windows.Forms.Button StopComputingButton;
		private System.Windows.Forms.ToolStripStatusLabel LastMoveStatusLabel;
		private System.Windows.Forms.ToolStripStatusLabel TurnPlayerStatusLabel;
		private System.Windows.Forms.ToolStripMenuItem CopyBoardPictureToolStripMenuItem;
		private System.Windows.Forms.ToolStripStatusLabel StepStatusLabel;
		private System.Windows.Forms.ComboBox ShowMoveNumberComboBox;
		private System.Windows.Forms.ToolStripMenuItem CopyBoardandTurnTextAToolStripMenuItem;
		private System.Windows.Forms.ToolStripStatusLabel ElapsedTimeStatusLabel;
	}
}

