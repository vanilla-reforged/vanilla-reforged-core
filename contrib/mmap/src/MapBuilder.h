/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MAP_BUILDER_H
#define _MAP_BUILDER_H

#include "TerrainBuilder.h"
#include "IntermediateValues.h"

#include "IVMapManager.h"

#include "Recast.h"
#include "DetourNavMesh.h"

#include "TileWorker.h"

using namespace VMAP;
// G3D namespace typedefs conflicts with ACE typedefs

using json = nlohmann::json;

namespace MMAP
{
    // these are WORLD UNIT based metrics
    // this are basic unit dimentions
    // value have to divide GRID_SIZE(533.33333f) ( aka: 0.5333, 0.2666, 0.3333, 0.1333, etc )
    const static float BASE_UNIT_DIM = 0.2666666f;

    // All are in UNIT metrics!
    const static int VERTEX_PER_MAP = int(GRID_SIZE / BASE_UNIT_DIM + 0.5f);
    const static int VERTEX_PER_TILE = 80; // must divide VERTEX_PER_MAP
    const static int TILES_PER_MAP = VERTEX_PER_MAP / VERTEX_PER_TILE;

    typedef std::map<uint32, std::set<uint32>> TileList;
    struct Tile
    {
        Tile() : chf(NULL), solid(NULL), cset(NULL), pmesh(NULL), dmesh(NULL) {}
        ~Tile()
        {
            rcFreeCompactHeightfield(chf);
            rcFreeContourSet(cset);
            rcFreeHeightField(solid);
            rcFreePolyMesh(pmesh);
            rcFreePolyMeshDetail(dmesh);
        }

        rcCompactHeightfield* chf;
        rcHeightfield* solid;
        rcContourSet* cset;
        rcPolyMesh* pmesh;
        rcPolyMeshDetail* dmesh;
    };

    class MapBuilder
    {
        friend class TileWorker;

        public:
            MapBuilder(char const* configInputPath,
                       bool skipLiquid          = false,
                       bool skipContinents      = false,
                       bool skipJunkMaps        = true,
                       bool skipBattlegrounds   = false,
                       bool debug               = false,
                       bool quick               = false,
                       const char* offMeshFilePath = nullptr,
                       uint8 threads = 1);

            ~MapBuilder();

            // builds an mmap tile for the specified map and its mesh
            void buildSingleTile(uint32 mapID, uint32 tileX, uint32 tileY);

            // builds list of maps, then builds all of mmap tiles (based on the skip settings)
            void buildAllMaps();
            // builds all mmap tiles for the specified map id (ignores skip map id settings)
            void buildSingleMap(uint32 mapID);

            void buildGameObject(std::string modelName, uint32 displayId);
            void buildTransports();

            bool IsBusy();

        private:
            // detect maps and tiles
            void discoverTiles();
            std::set<uint32>& getTileList(uint32 mapID);

            void buildMap(uint32 mapID);
            void buildNavMesh(uint32 mapID, dtNavMesh*& navMesh);

            void getTileBounds(uint32 tileX, uint32 tileY, float* verts, int vertCount, float* bmin, float* bmax);
            void getGridBounds(uint32 mapID, uint32& minX, uint32& minY, uint32& maxX, uint32& maxY);

            bool shouldSkipMap(uint32 mapID);
            bool isTransportMap(uint32 mapID);

            TerrainBuilder* m_terrainBuilder;
            TileList m_tiles;

            bool m_debug;

            const char* m_offMeshFilePath;
            bool m_skipContinents;
            bool m_skipJunkMaps;
            bool m_skipBattlegrounds;
            bool m_quick;
            json m_config;

            // build performance - not really used for now
            rcContext* m_rcContext;

            ProducerConsumerQueue<TileInfo> m_tileQueue;
            std::atomic<bool> m_cancel;
            uint8 m_threads;
    };
}

#endif
