#ifndef _OWN_MAP
#define _OWN_MAP

#include <map>

namespace utils {
	template<typename KT_,typename VT_>
	class Map {
	private:
		std::map<KT_, VT_> map;
	public:
		void add(const KT_& key,const VT_& obj) {
			if (!keyExists(key)) {
				map[key] = obj;
			}
		}

		void remove(const KT_& key) {
			map.erase(map.find(key));
		}
		void removeInd(size_t ind) {
			map.erase(getIndPtr(ind));
		}

		bool keyExists(const KT_& key) const {
			return map.find(key) != map.end();
		}

		VT_& get(const KT_& key) {
			return map.at(key);
		}
		const VT_& get(const KT_& key) const {
			return map.at(key);
		}

		VT_& atInd(size_t ind) {
			return this->getIndPtr(ind)->second;
		}
		const VT_& atInd(size_t ind) const {
			return this->getItemIndC(ind).second;
		}

		auto& getItemInd(size_t ind) {
			return *getIndPtr(ind);
		}
		const auto& getItemIndC(size_t ind) const {
			auto ptr = getIndPtr(ind);
			return *ptr;
		}

		auto getIndPtr(size_t ind) {
			auto it = map.begin();
			std::advance(it, ind);
			return it;
		}
		const auto getIndPtr(size_t ind) const {
			auto it = map.begin();
			std::advance(it, ind);
			return it;
		}

		auto begin() {
			return map.begin();
		}
		const auto begin() const {
			return map.begin();
		}

		auto end() {
			return map.end();
		}
		const auto end() const {
			return map.end();
		}

		size_t size() const {
			return map.size();
		}
	};
}


#endif