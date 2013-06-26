#ifndef __ENTITYMAP_H__
#define __ENTITYMAP_H__

template <typename TEntity>
class EntityMap {
	typedef std::map<std::string, TEntity> TTopicEntityMap;
	typedef std::map<::DDS::DomainId_t, TTopicEntityMap> TDomainEntityMap;

	TDomainEntityMap map_;

protected:
	virtual TEntity Create(DDSBase *ddsBase, ::DDS::DomainId_t domainId, const char *topic_name) = 0;

public:
	TEntity Get(DDSBase *ddsBase,::DDS::DomainId_t domainId, const char *topic_name) {
		TDomainEntityMap::iterator itDomain = map_.find(domainId);
		if (itDomain == map_.end())  // domain entry not found, so create topic map and entity
			map_[domainId][topic_name] = Create(ddsBase, domainId, topic_name);
		else {
			TTopicEntityMap::iterator itTopic = map_[domainId].find(topic_name);
			if (itTopic == map_[domainId].end())  // domain found, but topic not found, so create entity
				map_[domainId][topic_name] = Create(ddsBase, domainId, topic_name);
		}
		return map_[domainId][topic_name];
	}
};

#endif
