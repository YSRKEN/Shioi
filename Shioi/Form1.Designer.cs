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
			this.CopyBoardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
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
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.MenuStrip.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
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
			this.MenuStrip.Size = new System.Drawing.Size(384, 24);
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
			this.NewToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
			this.NewToolStripMenuItem.Text = "New(&N)";
			this.NewToolStripMenuItem.Click += new System.EventHandler(this.NewToolStripMenuItem_Click);
			// 
			// OpenToolStripMenuItem
			// 
			this.OpenToolStripMenuItem.Name = "OpenToolStripMenuItem";
			this.OpenToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
			this.OpenToolStripMenuItem.Text = "Open(&O)...";
			this.OpenToolStripMenuItem.Click += new System.EventHandler(this.OpenToolStripMenuItem_Click);
			// 
			// SaveToolStripMenuItem
			// 
			this.SaveToolStripMenuItem.Name = "SaveToolStripMenuItem";
			this.SaveToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
			this.SaveToolStripMenuItem.Text = "Save(&A)...";
			this.SaveToolStripMenuItem.Click += new System.EventHandler(this.SaveToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(126, 6);
			// 
			// ExitToolStripMenuItem
			// 
			this.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem";
			this.ExitToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
			this.ExitToolStripMenuItem.Text = "Exit(&X)";
			this.ExitToolStripMenuItem.Click += new System.EventHandler(this.ExitToolStripMenuItem_Click);
			// 
			// EditToolStripMenuItem
			// 
			this.EditToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CopyMoveToolStripMenuItem,
            this.PasteMoveToolStripMenuItem,
            this.CopyBoardToolStripMenuItem});
			this.EditToolStripMenuItem.Name = "EditToolStripMenuItem";
			this.EditToolStripMenuItem.Size = new System.Drawing.Size(53, 20);
			this.EditToolStripMenuItem.Text = "Edit(&E)";
			// 
			// CopyMoveToolStripMenuItem
			// 
			this.CopyMoveToolStripMenuItem.Name = "CopyMoveToolStripMenuItem";
			this.CopyMoveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
			this.CopyMoveToolStripMenuItem.Text = "Copy Move(&C)";
			this.CopyMoveToolStripMenuItem.Click += new System.EventHandler(this.CopyMoveToolStripMenuItem_Click);
			// 
			// PasteMoveToolStripMenuItem
			// 
			this.PasteMoveToolStripMenuItem.Name = "PasteMoveToolStripMenuItem";
			this.PasteMoveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
			this.PasteMoveToolStripMenuItem.Text = "Paste Move(&P)";
			this.PasteMoveToolStripMenuItem.Click += new System.EventHandler(this.PasteMoveToolStripMenuItem_Click);
			// 
			// CopyBoardToolStripMenuItem
			// 
			this.CopyBoardToolStripMenuItem.Name = "CopyBoardToolStripMenuItem";
			this.CopyBoardToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
			this.CopyBoardToolStripMenuItem.Text = "Copy Board(&B)";
			this.CopyBoardToolStripMenuItem.Click += new System.EventHandler(this.CopyBoardToolStripMenuItem_Click);
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
			this.ForwardToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
			this.ForwardToolStripMenuItem.Text = "-> Forward(&O)";
			this.ForwardToolStripMenuItem.Click += new System.EventHandler(this.ForwardToolStripMenuItem_Click);
			// 
			// BackwardToolStripMenuItem
			// 
			this.BackwardToolStripMenuItem.Name = "BackwardToolStripMenuItem";
			this.BackwardToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
			this.BackwardToolStripMenuItem.Text = "<- Backward(&B)";
			this.BackwardToolStripMenuItem.Click += new System.EventHandler(this.BackwardToolStripMenuItem_Click);
			// 
			// FirstMoveToolStripMenuItem
			// 
			this.FirstMoveToolStripMenuItem.Name = "FirstMoveToolStripMenuItem";
			this.FirstMoveToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
			this.FirstMoveToolStripMenuItem.Text = "|<- First move(&F)";
			this.FirstMoveToolStripMenuItem.Click += new System.EventHandler(this.FirstMoveToolStripMenuItem_Click);
			// 
			// LastMoveToolStripMenuItem
			// 
			this.LastMoveToolStripMenuItem.Name = "LastMoveToolStripMenuItem";
			this.LastMoveToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
			this.LastMoveToolStripMenuItem.Text = "->| Last move(&L)";
			this.LastMoveToolStripMenuItem.Click += new System.EventHandler(this.LastMoveToolStripMenuItem_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(184, 6);
			// 
			// StartComputingToolStripMenuItem
			// 
			this.StartComputingToolStripMenuItem.Name = "StartComputingToolStripMenuItem";
			this.StartComputingToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
			this.StartComputingToolStripMenuItem.Text = "|> Start computing(&S)";
			this.StartComputingToolStripMenuItem.Click += new System.EventHandler(this.StartComputingToolStripMenuItem_Click);
			// 
			// StopComputingToolStripMenuItem
			// 
			this.StopComputingToolStripMenuItem.Name = "StopComputingToolStripMenuItem";
			this.StopComputingToolStripMenuItem.Size = new System.Drawing.Size(187, 22);
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
			this.StatusStrip.Location = new System.Drawing.Point(0, 339);
			this.StatusStrip.Name = "StatusStrip";
			this.StatusStrip.Size = new System.Drawing.Size(384, 22);
			this.StatusStrip.TabIndex = 1;
			this.StatusStrip.Text = "StatusStrip";
			// 
			// pictureBox1
			// 
			this.pictureBox1.Location = new System.Drawing.Point(12, 27);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(360, 309);
			this.pictureBox1.TabIndex = 2;
			this.pictureBox1.TabStop = false;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(384, 361);
			this.Controls.Add(this.pictureBox1);
			this.Controls.Add(this.StatusStrip);
			this.Controls.Add(this.MenuStrip);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MainMenuStrip = this.MenuStrip;
			this.MaximizeBox = false;
			this.Name = "MainForm";
			this.Text = "Shioi";
			this.MenuStrip.ResumeLayout(false);
			this.MenuStrip.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
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
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.ToolStripMenuItem EditToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem CopyMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem PasteMoveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem CopyBoardToolStripMenuItem;
	}
}

