#pragma once

namespace EditorFileDialog
{
	const wchar_t* OpenFileDialog(int FormatType, int SelectedFormat = 1);
	const wchar_t* SaveFileDialog(int FormatType, int SelectedFormat = 1);
}