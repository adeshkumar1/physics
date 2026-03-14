#pragma once

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

class SpatialGrid {
 private:
	float cellSize;
	int cols, rows;
	float offsetX, offsetY;
	std::vector<std::vector<size_t>> cells;

	int cellIndex(int col, int row) const { return row * cols + col; }

 public:
	void configure(float worldWidth, float worldHeight, float worldCenterX,
	               float worldCenterY, float maxRadius) {
		cellSize = maxRadius * 4.0f;
		if (cellSize < 1.0f) cellSize = 60.0f;

		offsetX = worldCenterX - worldWidth / 2.0f;
		offsetY = worldCenterY - worldHeight / 2.0f;

		cols = static_cast<int>(std::ceil(worldWidth / cellSize)) + 1;
		rows = static_cast<int>(std::ceil(worldHeight / cellSize)) + 1;

		cells.resize(static_cast<size_t>(cols * rows));
	}

	void clear() {
		for (auto &cell : cells) {
			cell.clear();
		}
	}

	void insert(size_t index, float x, float y, float radius) {
		int minCol = static_cast<int>((x - radius - offsetX) / cellSize);
		int maxCol = static_cast<int>((x + radius - offsetX) / cellSize);
		int minRow = static_cast<int>((y - radius - offsetY) / cellSize);
		int maxRow = static_cast<int>((y + radius - offsetY) / cellSize);

		minCol = std::max(0, minCol);
		maxCol = std::min(cols - 1, maxCol);
		minRow = std::max(0, minRow);
		maxRow = std::min(rows - 1, maxRow);

		for (int r = minRow; r <= maxRow; r++) {
			for (int c = minCol; c <= maxCol; c++) {
				cells[static_cast<size_t>(cellIndex(c, r))].push_back(index);
			}
		}
	}

	template <typename Callback>
	void forEachPair(Callback &&callback) const {
		for (const auto &cell : cells) {
			for (size_t i = 0; i < cell.size(); i++) {
				for (size_t j = i + 1; j < cell.size(); j++) {
					size_t a = cell[i], b = cell[j];
					if (a > b) std::swap(a, b);
					callback(a, b);
				}
			}
		}
	}
};
