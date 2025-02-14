void renderTile(const Tile& tile) {
    if (tile.isWall) {
        glBegin(GL_QUADS);
        glVertex2f(tile.x, tile.y);
        glVertex2f(tile.x + TILE_SIZE, tile.y);
        glVertex2f(tile.x + TILE_SIZE, tile.y + TILE_SIZE);
        glVertex2f(tile.x, tile.y + TILE_SIZE);
        glEnd();
    }
}