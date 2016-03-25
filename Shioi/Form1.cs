using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Shioi {
	public partial class MainForm : Form {
		Renju FormRenju;

		public MainForm() {
			InitializeComponent();
			FormRenju = new Renju();
		}

		private void NewToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void OpenToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void SaveToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void ExitToolStripMenuItem_Click(object sender, EventArgs e) {
			this.Close();
		}

		private void ForwardToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void BackwardToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void FirstMoveToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void LastMoveToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void StartComputingToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void StopComputingToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void AboutAToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private class Renju {
			// 各種定数
			enum Stone {
				None,
				Black,
				White
			}
			const int BoardSize = 15;
			// メンバ変数
			List<Stone> Board;
			List<int> Move;
			int MovePointer;
			// メソッド
			public Renju() {
				Board = new List<Stone>(BoardSize * BoardSize);
			}
		}
	}
}
