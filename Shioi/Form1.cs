using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Shioi {
	public partial class MainForm : Form {
		Renju FormRenju;
		const string SoftName = "Shioi";
		const int BoardSize = 15;
		const string PositionStringX = "abcdefghijklmno";
		const string PositionStringY = "123456789ABCDEF";
		public enum Stone {
			None,
			Black,
			White
		}

		public MainForm() {
			InitializeComponent();
			FormRenju = new Renju();
			DrawBoard();
		}

		private void NewToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju = new Renju();
		}

		private void OpenToolStripMenuItem_Click(object sender, EventArgs e) {
			// Show "open file dialog"
			var ofd = new OpenFileDialog();
			ofd.Filter = "Gomoku Kyousou(*.ban)|*.ban|Date Gomoku(*.dt5)|*.dt5|Fiver6(*.lws)|*.lws|Renju Kozou(*.rnj)|*.rnj";
			if(ofd.ShowDialog() != DialogResult.OK)
				return;
			// Open select file
			FormRenju = new Renju(ofd.FileName);
			DrawBoard();
		}

		private void SaveToolStripMenuItem_Click(object sender, EventArgs e) {
			// Show "save file dialog"
			var sfd = new SaveFileDialog();
			sfd.Filter = "Gomoku Kyousou(*.ban)|*.ban|Date Gomoku(*.dt5)|*.dt5|Fiver6(*.lws)|*.lws|Renju Kozou(*.rnj)|*.rnj";
			if(sfd.ShowDialog() != DialogResult.OK)
				return;
			// Save select file
			FormRenju.SaveFile(sfd.FileName);
		}

		private void ExitToolStripMenuItem_Click(object sender, EventArgs e) {
			this.Close();
		}

		private void CopyMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			var output = FormRenju.ToStringMove();
			Clipboard.SetDataObject(output, true);
		}

		private void PasteMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			var data = Clipboard.GetDataObject();
			if(data.GetDataPresent(DataFormats.Text)) {
				var input = (string)data.GetData(DataFormats.Text);
				FormRenju.Parse(input);
				LastMoveToolStripMenuItem_Click(sender, e);
			}
		}

		private void CopyBoardTextToolStripMenuItem_Click(object sender, EventArgs e) {
			var output = FormRenju.ToStringBoard();
			Clipboard.SetDataObject(output, true);
		}

		private void CopyBoardPictureToolStripMenuItem_Click(object sender, EventArgs e) {
			var bmp = new Bitmap(PictureBox.Image);
			Clipboard.SetImage(bmp);
		}

		private void ForwardToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju.Next();
			DrawBoard();
		}

		private void BackwardToolStripMenuItem_Click(object sender, EventArgs e) {
			FormRenju.Prev();
			DrawBoard();
		}

		private void FirstMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			int pointer = FormRenju.MovePointer;
			for(int i = 0; i <= pointer; ++i)
				FormRenju.Prev();
			DrawBoard();
		}

		private void LastMoveToolStripMenuItem_Click(object sender, EventArgs e) {
			int pointer = FormRenju.MovePointer;
			for(int i = pointer; i < FormRenju.Move.Count; ++i)
				FormRenju.Next();
			DrawBoard();
		}

		private void StartComputingToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void StopComputingToolStripMenuItem_Click(object sender, EventArgs e) {

		}

		private void FirstMoveButton_Click(object sender, EventArgs e) {
			FirstMoveToolStripMenuItem_Click(sender, e);
		}

		private void BackwardButton_Click(object sender, EventArgs e) {
			BackwardToolStripMenuItem_Click(sender, e);
		}

		private void ForwardButton_Click(object sender, EventArgs e) {
			ForwardToolStripMenuItem_Click(sender, e);
		}

		private void LastMoveButton_Click(object sender, EventArgs e) {
			LastMoveToolStripMenuItem_Click(sender, e);
		}

		private void StartComputingButton_Click(object sender, EventArgs e) {

		}

		private void StopComputingButton_Click(object sender, EventArgs e) {

		}

		private void AboutAToolStripMenuItem_Click(object sender, EventArgs e) {
			var assem = Assembly.GetExecutingAssembly();
			var v = assem.GetName().Version;
			var version = v.Major.ToString() + "." + v.Minor.ToString() + "." + v.Build.ToString() + "." + v.Revision.ToString();
			var assemblyProduct = ((AssemblyProductAttribute)Attribute.GetCustomAttribute(assem, typeof(AssemblyProductAttribute))).Product;
			var assemblyCopyright = ((AssemblyCopyrightAttribute)Attribute.GetCustomAttribute(assem, typeof(AssemblyCopyrightAttribute))).Copyright;
			var verInfo = 
				"SoftName : " + assemblyProduct + "\n"
				+ "Version : " + version + "\n"
				+ "Created by YSR\n"
				+ assemblyCopyright;
			MessageBox.Show(verInfo, SoftName);
		}

		private void PictureBox_MouseClick(object sender, MouseEventArgs e) {
			var blockSize = PictureBox.Width / 16;
			var x = e.Location.X / blockSize - 1;
			var y = e.Location.Y / blockSize - 1;
			if(FormRenju.Point(x, y) != Stone.None)
				return;
			var p = Renju.ConvertMove2to1(x, y);
			FormRenju.Board[p] = FormRenju.TurnPlayer();
			++FormRenju.MovePointer;
			FormRenju.Move.RemoveRange(FormRenju.MovePointer, FormRenju.Move.Count - FormRenju.MovePointer);
			FormRenju.Move.Add(p);
			DrawBoard();
		}

		private void DrawBoard() {
			// 盤面を描画する
			var canvas = new Bitmap(PictureBox.Width, PictureBox.Height);
			var blockSize = PictureBox.Width / 16;
			var boardOffset = blockSize * 3 / 8;
			var g = Graphics.FromImage(canvas);
			// 背景
			g.FillRectangle(Brushes.Gold, 0, 0, PictureBox.Width, PictureBox.Height);
			// 枠線
			for(int y = 1; y <= BoardSize; ++y) {
				g.DrawLine(Pens.Black, blockSize + boardOffset, blockSize * y + boardOffset, blockSize * BoardSize + boardOffset, blockSize * y + boardOffset);
			}
			for(int x = 1; x <= BoardSize; ++x) {
				g.DrawLine(Pens.Black, blockSize * x + boardOffset, blockSize + boardOffset, blockSize * x + boardOffset, blockSize * BoardSize + boardOffset);
			}
			// 座標
			var fontSize = blockSize / 2;
			var font = new Font("MS Gothic", fontSize, FontStyle.Bold);
			for(int y = 0; y < BoardSize; ++y) {
				g.DrawString(PositionStringY.Substring(y, 1), font, Brushes.Black, blockSize / 8, blockSize * y + blockSize * 3 / 4 + boardOffset);
			}
			for(int x = 0; x < BoardSize; ++x) {
				g.DrawString(PositionStringX.Substring(x, 1), font, Brushes.Black, blockSize * x + blockSize * 3 / 4 + boardOffset, blockSize / 8);
			}
			// 星
			const int StarR = 3;
			g.FillEllipse(Brushes.Black, blockSize *  4 - StarR + boardOffset, blockSize *  4 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize * 12 - StarR + boardOffset, blockSize *  4 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize * 12 - StarR + boardOffset, blockSize * 12 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize *  4 - StarR + boardOffset, blockSize * 12 - StarR + boardOffset, StarR * 2, StarR * 2);
			g.FillEllipse(Brushes.Black, blockSize *  8 - StarR + boardOffset, blockSize *  8 - StarR + boardOffset, StarR * 2, StarR * 2);
			// 石
			var StoneR = blockSize / 2;
			var font2 = new Font("MS Gothic", fontSize - 2, FontStyle.Bold);
			for(int p = 0; p <= FormRenju.MovePointer; ++p) {
				var MoveXY = Renju.ConvertMove1to2(FormRenju.Move[p]);
				var drawX = blockSize * MoveXY[0] + StoneR + 1 + boardOffset;
				var drawY = blockSize * MoveXY[1] + StoneR + 1 + boardOffset;
				var pLength = (p + 1).ToString().Length;
				var pLengthOffset = pLength * (fontSize - 2) / 2;
				int[] offsetX1 = {0, fontSize, fontSize * 19 / 16, fontSize * 5 / 4}; 
				var offsetX = - pLengthOffset + offsetX1[pLength];
				var offsetY = fontSize / 2;
				switch(FormRenju.Point(p)) {
				case Stone.None:
					break;
				case Stone.Black:
					g.FillEllipse(Brushes.Black, drawX, drawY, StoneR * 2, StoneR * 2);
					g.DrawString((p + 1).ToString(), font2, Brushes.White, drawX + offsetX, drawY + offsetY);
					break;
				case Stone.White:
					g.FillEllipse(Brushes.White, drawX, drawY, StoneR * 2, StoneR * 2);
					g.DrawEllipse(Pens.Black, drawX, drawY, StoneR * 2, StoneR * 2);
					g.DrawString((p + 1).ToString(), font2, Brushes.Black, drawX + offsetX, drawY + offsetY);
					break;
				}
			}
			g.Dispose();
			PictureBox.Image = canvas;
			// ステータスバーを変更する
			LastMoveStatusLabel.Text = FormRenju.GetLastMoveText();
			TurnPlayerStatusLabel.Text = FormRenju.GetTurnPlayerText();
			StepStatusLabel.Text = FormRenju.GetStepText();
		}

		private class Renju {
			// メンバ変数
			public List<Stone> Board;
			public List<int> Move;
			public int MovePointer;
			// メソッド
			// コンストラクタ
			public Renju() {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
			}
			public Renju(string FileName) {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
				System.IO.StreamReader sr = new System.IO.StreamReader(FileName, System.Text.Encoding.GetEncoding("utf-8"));
				var fileData = sr.ReadToEnd();
				switch(Path.GetExtension(FileName)) {
				case ".ban": {
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						var setStone = Stone.Black;
						foreach(var str in getLine) {
							if(str.Length < 3)
								break;
							var x = int.Parse(str.Substring(0, str.Length - 2)) - 1;
							var y = int.Parse(str.Substring(str.Length - 2, 2)) - 1;
							if(x < 0 || y < 0)
								break;
							var move = ConvertMove2to1(x, y);
							Move.Add(move);
							Board[move] = setStone;
							setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".dt5":
					Parse(fileData);
					break;
				case ".lws":
					{
						const string PositionStringX = "ABCDEFGHIJKLMNO";
						var moveStr = fileData.Split(' ');
						var setStone = Stone.Black;
						foreach(var str in moveStr) {
							var x = PositionStringX.IndexOf(str[0]);
							var y = BoardSize - int.Parse(str.Substring(1));
							if(x < 0)
								break;
							var move = ConvertMove2to1(x, y);
							Move.Add(move);
							Board[move] = setStone;
							setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
						}
						MovePointer = Move.Count - 1;
					}
					break;
				case ".rnj": {
						var getLine = fileData.Replace("\r\n", "\n").Split('\n');
						foreach(var str in getLine) {
							var info = str.Split(' ');
							if(info.Count() < 2)
								break;
							var x1 = int.Parse(info[1]);
							var y1 = int.Parse(info[0]);
							if(x1 < 0 || y1 < 0)
								break;
							var move1 = ConvertMove2to1(x1, y1);
							Move.Add(move1);
							Board[move1] = Stone.Black;
							if(info.Count() < 4)
								break;
							var x2 = int.Parse(info[3]);
							var y2 = int.Parse(info[2]);
							if(x2 < 0 || y2 < 0)
								break;
							var move2 = ConvertMove2to1(x2, y2);
							Move.Add(move2);
							Board[move2] = Stone.White;
						}
						MovePointer = Move.Count - 1;
					}
					break;
				}
				return;
			}
			// ファイル書き込み
			public void SaveFile(string FileName){
				// 出力用文字列
				var fileData = "";
				switch(Path.GetExtension(FileName)) {
				case ".ban":
					{
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += (moveXY[0] + 1).ToString() + (moveXY[1] + 1).ToString("D2") + "\r\n";
						}
					}
					break;
				case ".dt5":
					{
						fileData = ToStringMove();
					}
					break;
				case ".lws":
					{
						const string PositionStringX = "ABCDEFGHIJKLMNO";
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							if(p != 0)
								fileData += " ";
							fileData += PositionStringX[moveXY[0]] + (BoardSize - moveXY[1]).ToString();
						}
					}
					break;
				case ".rnj":
					{
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += moveXY[1].ToString() + " " + moveXY[0].ToString();
							fileData += (p % 2 == 0 ? " " : "\r\n");
						}
					}
					break;
				}
				var sw = new StreamWriter(FileName, false, Encoding.GetEncoding("shift-jis"));
				sw.Write(fileData);
				sw.Close();
			}
			// 盤面を文字列に変換する
			public string ToStringBoard() {
				// 星の座標
				List<int> StarPosition = new List<int> { ConvertMove2to1(3, 3), ConvertMove2to1(11, 3), ConvertMove2to1(11, 11), ConvertMove2to1(3, 11), ConvertMove2to1(7, 7) };
				// 出力用文字列
				var output = "";
				for(int y = 0; y < BoardSize; ++y) {
					for(int x = 0; x < BoardSize; ++x) {
						var position = ConvertMove2to1(x, y);
						switch(Board[position]) {
						case Stone.None:
							if(x == 0 && y == 0) {
								output += "┌";
							} else if(x == BoardSize - 1 && y == 0) {
								output += "┐";
							} else if(x == BoardSize - 1 && y == BoardSize - 1) {
								output += "┘";
							} else if(x == 0 && y == BoardSize - 1) {
								output += "└";
							} else if(y == 0) {
								output += "┬";
							} else if(x == BoardSize - 1) {
								output += "┤";
							} else if(y == BoardSize - 1) {
								output += "┴";
							} else if(x == 0) {
								output += "├";
							} else if(StarPosition.IndexOf(position) != -1) {
								output += "╋";
							} else {
								output += "┼";
							}
							break;
						case Stone.Black:
							output += "●";
							break;
						case Stone.White:
							output += "○";
							break;
						}
					}
					output += "\n";
				}
				return output;
			}
			// 着手を文字列に変換する(伊達五目形式)
			private string ToStringMoveMini(int Move) {
				var moveXY = ConvertMove1to2(Move);
				return PositionStringX.Substring(moveXY[0], 1) + PositionStringY.Substring(moveXY[1], 1);
			}
			public string ToStringMove() {
				var output = "";
				for(int p = 0; p <= MovePointer; ++p) {
					output += ToStringMoveMini(Move[p]) + ",";
				}
				output += "**,";
				return output;
			}
			// 文字列を着手に変換して読み込む(伊達五目形式)
			public void Parse(string input) {
				Board = new List<Stone>(Enumerable.Repeat(Stone.None, BoardSize * BoardSize));
				Move = new List<int>();
				MovePointer = -1;
				const string PositionStringX = "abcdefghijklmno";
				const string PositionStringY = "123456789ABCDEF";
				var moveStr = input.Split(',');
				var setStone = Stone.Black;
				foreach(var str in moveStr) {
					var x = PositionStringX.IndexOf(str[0]);
					var y = PositionStringY.IndexOf(str[1]);
					if(x < 0 || y < 0)
						break;
					var move = ConvertMove2to1(x, y);
					Move.Add(move);
					Board[move] = setStone;
					setStone = (setStone == Stone.Black ? Stone.White : Stone.Black);
				}
				MovePointer = Move.Count - 1;
			}
			// 盤面を表示する(デバッグ用)
			public void PutBoard() {
				MessageBox.Show(ToStringBoard(), SoftName, MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
			// 座標変換
			static public int ConvertMove2to1(int MoveX, int MoveY) {
				return MoveY * BoardSize + MoveX;
			}
			static public int[] ConvertMove1to2(int Move) {
				return new int[]{ Move % BoardSize, Move / BoardSize };
			}
			// 手番情報
			public Stone TurnPlayer() {
				return (MovePointer % 2 == 0 ? Stone.White : Stone.Black);
			}
			// 盤面の情報
			public Stone Point(int p) {
				return Board[Move[p]];
			}
			public Stone Point(int x, int y) {
				return Board[Renju.ConvertMove2to1(x, y)];
			}
			// 手番を進める・戻す
			public void Next() {
				if(MovePointer >= Move.Count - 1)
					return;
				var setStone = TurnPlayer();
				++MovePointer;
				Board[Move[MovePointer]] = setStone;
			}
			public void Prev() {
				if(MovePointer < 0)
					return;
				Board[Move[MovePointer]] = Stone.None;
				--MovePointer;
			}
			// ステータスバー用文字列の生成
			public string GetLastMoveText() {
				return "LastMove : " + (Move.Count == 0 | MovePointer == -1 ? "" : ToStringMoveMini(Move[MovePointer]));
			}
			public string GetTurnPlayerText() {
				return "Turn : " + (TurnPlayer() == Stone.Black ? "Black" : "White");
			}
			public string GetStepText() {
				return "Step : " + (MovePointer + 1).ToString() + " / " + Move.Count.ToString();
			}
		}
	}
}
