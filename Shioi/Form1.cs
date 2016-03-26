using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Shioi {
	public partial class MainForm : Form {
		Renju FormRenju;
		const string SoftName = "Shiori";

		public MainForm() {
			InitializeComponent();
			FormRenju = new Renju();
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
			FormRenju.PutBoard();
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
				case ".dt5": {
						const string PositionStringX = "abcdefghijklmno";
						const string PositionStringY = "123456789ABCDEF";
						var moveStr = fileData.Split(',');
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
					}
					break;
				case ".lws": {
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
						const string PositionStringX = "abcdefghijklmno";
						const string PositionStringY = "123456789ABCDEF";
						for(int p = 0; p <= MovePointer; ++p) {
							var moveXY = ConvertMove1to2(Move[p]);
							fileData += PositionStringX.Substring(moveXY[0], 1) + PositionStringY.Substring(moveXY[1], 1) + ",";
						}
						fileData += "**,";
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
			// 盤面を表示する(デバッグ用)
			public void PutBoard() {
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
				MessageBox.Show(output, SoftName, MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
			// 座標変換
			private int ConvertMove2to1(int MoveX, int MoveY) {
				return MoveY * BoardSize + MoveX;
			}
			private int[] ConvertMove1to2(int Move) {
				return new int[]{ Move % BoardSize, Move / BoardSize };
			}
		}
	}
}
