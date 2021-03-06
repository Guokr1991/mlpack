/**
 * @file lsh_search.hpp
 * @author Parikshit Ram
 *
 * Defines the LSHSearch class, which performs an approximate
 * nearest neighbor search for a queries in a query set
 * over a given dataset using Locality-sensitive hashing
 * with 2-stable distributions.
 *
 * The details of this method can be found in the following paper:
 *
 * @inproceedings{datar2004locality,
 *  title={Locality-sensitive hashing scheme based on p-stable distributions},
 *  author={Datar, M. and Immorlica, N. and Indyk, P. and Mirrokni, V.S.},
 *  booktitle=
 *      {Proceedings of the 12th Annual Symposium on Computational Geometry},
 *  pages={253--262},
 *  year={2004},
 *  organization={ACM}
 * }
 *
 */
#ifndef MLPACK_METHODS_NEIGHBOR_SEARCH_LSH_SEARCH_HPP
#define MLPACK_METHODS_NEIGHBOR_SEARCH_LSH_SEARCH_HPP

#include <mlpack/core.hpp>
#include <vector>
#include <string>

#include <mlpack/core/metrics/lmetric.hpp>
#include <mlpack/methods/neighbor_search/sort_policies/nearest_neighbor_sort.hpp>

namespace mlpack {
namespace neighbor {

/**
 * The LSHSearch class; this class builds a hash on the reference set and uses
 * this hash to compute the distance-approximate nearest-neighbors of the given
 * queries.
 *
 * @tparam SortPolicy The sort policy for distances; see NearestNeighborSort.
 */
template<typename SortPolicy = NearestNeighborSort>
class LSHSearch
{
 public:
  /**
   * This function initializes the LSH class. It builds the hash on the
   * reference set with 2-stable distributions. See the individual functions
   * performing the hashing for details on how the hashing is done.
   *
   * @param referenceSet Set of reference points and the set of queries.
   * @param projections Cube of projection tables. For a cube of size (a, b, c)
   *     we set numProj = a, numTables = c. b is the reference set
   *     dimensionality.
   * @param hashWidth The width of hash for every table. If 0 (the default) is
   *     provided, then the hash width is automatically obtained by computing
   *     the average pairwise distance of 25 pairs.  This should be a reasonable
   *     upper bound on the nearest-neighbor distance in general.
   * @param secondHashSize The size of the second hash table. This should be a
   *     large prime number.
   * @param bucketSize The size of the bucket in the second hash table. This is
   *     the maximum number of points that can be hashed into single bucket.  A
   *     value of 0 indicates that there is no limit (so the second hash table
   *     can be arbitrarily large---be careful!).
   */
  LSHSearch(const arma::mat& referenceSet,
            const arma::cube& projections,
            const double hashWidth = 0.0,
            const size_t secondHashSize = 99901,
            const size_t bucketSize = 500);

  /**
   * This function initializes the LSH class. It builds the hash one the
   * reference set using the provided projections. See the individual functions
   * performing the hashing for details on how the hashing is done.
   *
   * @param referenceSet Set of reference points and the set of queries.
   * @param numProj Number of projections in each hash table (anything between
   *     10-50 might be a decent choice).
   * @param numTables Total number of hash tables (anything between 10-20
   *     should suffice).
   * @param hashWidth The width of hash for every table. If 0 (the default) is
   *     provided, then the hash width is automatically obtained by computing
   *     the average pairwise distance of 25 pairs.  This should be a reasonable
   *     upper bound on the nearest-neighbor distance in general.
   * @param secondHashSize The size of the second hash table. This should be a
   *     large prime number.
   * @param bucketSize The size of the bucket in the second hash table. This is
   *     the maximum number of points that can be hashed into single bucket.  A
   *     value of 0 indicates that there is no limit (so the second hash table
   *     can be arbitrarily large---be careful!).
   */
  LSHSearch(const arma::mat& referenceSet,
            const size_t numProj,
            const size_t numTables,
            const double hashWidth = 0.0,
            const size_t secondHashSize = 99901,
            const size_t bucketSize = 500);

  /**
   * Create an untrained LSH model.  Be sure to call Train() before calling
   * Search(); otherwise, an exception will be thrown when Search() is called.
   */
  LSHSearch();

  /**
   * Clean memory.
   */
  ~LSHSearch();

  /**
   * Train the LSH model on the given dataset.  If a correctly-sized projection
   * cube is not provided, this means building new hash tables. Otherwise, we
   * use the projections provided by the user.
   *
   * @param referenceSet Set of reference points and the set of queries.
   * @param numProj Number of projections in each hash table (anything between
   *     10-50 might be a decent choice).
   * @param numTables Total number of hash tables (anything between 10-20
   *     should suffice).
   * @param hashWidth The width of hash for every table. If 0 (the default) is
   *     provided, then the hash width is automatically obtained by computing
   *     the average pairwise distance of 25 pairs.  This should be a reasonable
   *     upper bound on the nearest-neighbor distance in general.
   * @param secondHashSize The size of the second hash table. This should be a
   *     large prime number.
   * @param bucketSize The size of the bucket in the second hash table. This is
   *     the maximum number of points that can be hashed into single bucket.  A
   *     value of 0 indicates that there is no limit (so the second hash table
   *     can be arbitrarily large---be careful!).
   * @param projections Cube of projection tables. For a cube of size (a, b, c)
   *     we set numProj = a, numTables = c. b is the reference set
   *     dimensionality.
   */
  void Train(const arma::mat& referenceSet,
             const size_t numProj,
             const size_t numTables,
             const double hashWidth = 0.0,
             const size_t secondHashSize = 99901,
             const size_t bucketSize = 500,
             const arma::cube& projection = arma::cube());

  /**
   * Compute the nearest neighbors of the points in the given query set and
   * store the output in the given matrices.  The matrices will be set to the
   * size of n columns by k rows, where n is the number of points in the query
   * dataset and k is the number of neighbors being searched for.
   *
   * @param querySet Set of query points.
   * @param k Number of neighbors to search for.
   * @param resultingNeighbors Matrix storing lists of neighbors for each query
   *     point.
   * @param distances Matrix storing distances of neighbors for each query
   *     point.
   * @param numTablesToSearch This parameter allows the user to have control
   *     over the number of hash tables to be searched. This allows
   *     the user to pick the number of tables it can afford for the time
   *     available without having to build hashing for every table size.
   *     By default, this is set to zero in which case all tables are
   *     considered.
   */
  void Search(const arma::mat& querySet,
              const size_t k,
              arma::Mat<size_t>& resultingNeighbors,
              arma::mat& distances,
              const size_t numTablesToSearch = 0);

  /**
   * Compute the nearest neighbors and store the output in the given matrices.
   * The matrices will be set to the size of n columns by k rows, where n is
   * the number of points in the query dataset and k is the number of neighbors
   * being searched for.
   *
   * @param k Number of neighbors to search for.
   * @param resultingNeighbors Matrix storing lists of neighbors for each query
   *     point.
   * @param distances Matrix storing distances of neighbors for each query
   *     point.
   * @param numTablesToSearch This parameter allows the user to have control
   *     over the number of hash tables to be searched. This allows
   *     the user to pick the number of tables it can afford for the time
   *     available without having to build hashing for every table size.
   *     By default, this is set to zero in which case all tables are
   *     considered.
   */
  void Search(const size_t k,
              arma::Mat<size_t>& resultingNeighbors,
              arma::mat& distances,
              const size_t numTablesToSearch = 0);

  /**
   * Serialize the LSH model.
   *
   * @param ar Archive to serialize to.
   */
  template<typename Archive>
  void Serialize(Archive& ar, const unsigned int version);

  //! Return the number of distance evaluations performed.
  size_t DistanceEvaluations() const { return distanceEvaluations; }
  //! Modify the number of distance evaluations performed.
  size_t& DistanceEvaluations() { return distanceEvaluations; }

  //! Return the reference dataset.
  const arma::mat& ReferenceSet() const { return *referenceSet; }

  //! Get the number of projections.
  size_t NumProjections() const { return projections.n_slices; }

  //! Get the offsets 'b' for each of the projections.  (One 'b' per column.)
  const arma::mat& Offsets() const { return offsets; }

  //! Get the weights of the second hash.
  const arma::vec& SecondHashWeights() const { return secondHashWeights; }

  //! Get the bucket size of the second hash.
  size_t BucketSize() const { return bucketSize; }

  //! Get the second hash table.
  const std::vector<arma::Col<size_t>>& SecondHashTable() const
      { return secondHashTable; }

  //! Get the projection tables.
  const arma::cube& Projections() { return projections; }

  //! Change the projection tables (this retrains the LSH model).
  void Projections(const arma::cube& projTables)
  {
    // Simply call Train() with the given projection tables.
    Train(*referenceSet, numProj, numTables, hashWidth, secondHashSize,
        bucketSize, projTables);
  }

  //! Get a single projection matrix.  This function is deprecated and will be
  //! removed in mlpack 2.1.0!
  const arma::mat& Projection(size_t i) { return projections.slice(i); }

 private:
  /**
   * This function takes a query and hashes it into each of the hash tables to
   * get keys for the query and then the key is hashed to a bucket of the second
   * hash table and all the points (if any) in those buckets are collected as
   * the potential neighbor candidates.
   *
   * @param queryPoint The query point currently being processed.
   * @param referenceIndices The list of neighbor candidates obtained from
   *    hashing the query into all the hash tables and eventually into
   *    multiple buckets of the second hash table.
   */
  template<typename VecType>
  void ReturnIndicesFromTable(const VecType& queryPoint,
                              arma::uvec& referenceIndices,
                              size_t numTablesToSearch) const;

  /**
   * This is a helper function that computes the distance of the query to the
   * neighbor candidates and appropriately stores the best 'k' candidates.  This
   * is specific to the monochromatic search case, where the query set is the
   * reference set.
   *
   * @param queryIndex The index of the query in question
   * @param referenceIndex The index of the neighbor candidate in question
   * @param neighbors Matrix holding output neighbors.
   * @param distances Matrix holding output distances.
   */
  void BaseCase(const size_t queryIndex,
                const size_t referenceIndex,
                arma::Mat<size_t>& neighbors,
                arma::mat& distances) const;

  /**
   * This is a helper function that computes the distance of the query to the
   * neighbor candidates and appropriately stores the best 'k' candidates.  This
   * is specific to bichromatic search, where the query set is not the same as
   * the reference set.
   *
   * @param queryIndex The index of the query in question
   * @param referenceIndex The index of the neighbor candidate in question
   * @param querySet Set of query points.
   * @param neighbors Matrix holding output neighbors.
   * @param distances Matrix holding output distances.
   */
  void BaseCase(const size_t queryIndex,
                const size_t referenceIndex,
                const arma::mat& querySet,
                arma::Mat<size_t>& neighbors,
                arma::mat& distances) const;

  /**
   * This is a helper function that efficiently inserts better neighbor
   * candidates into an existing set of neighbor candidates. This function is
   * only called by the 'BaseCase' function.
   *
   * @param distances Matrix holding output distances.
   * @param neighbors Matrix holding output neighbors.
   * @param queryIndex This is the index of the query being processed currently
   * @param pos The position of the neighbor candidate in the current list of
   *    neighbor candidates.
   * @param neighbor The neighbor candidate that is being inserted into the list
   *    of the best 'k' candidates for the query in question.
   * @param distance The distance of the query to the neighbor candidate.
   */
  void InsertNeighbor(arma::mat& distances,
                      arma::Mat<size_t>& neighbors,
                      const size_t queryIndex,
                      const size_t pos,
                      const size_t neighbor,
                      const double distance) const;

  //! Reference dataset.
  const arma::mat* referenceSet;
  //! If true, we own the reference set.
  bool ownsSet;

  //! The number of projections.
  size_t numProj;
  //! The number of hash tables.
  size_t numTables;

  //! The arma::cube containing the projection matrix of each table.
  arma::cube projections; // should be [numProj x dims] x numTables slices

  //! The list of the offsets 'b' for each of the projection for each table.
  arma::mat offsets; // should be numProj x numTables

  //! The hash width.
  double hashWidth;

  //! The big prime representing the size of the second hash.
  size_t secondHashSize;

  //! The weights of the second hash.
  arma::vec secondHashWeights;

  //! The bucket size of the second hash.
  size_t bucketSize;

  //! The final hash table; should be (< secondHashSize) vectors each with
  //! (<= bucketSize) elements.
  std::vector<arma::Col<size_t>> secondHashTable;

  //! The number of elements present in each hash bucket; should be
  //! secondHashSize.
  arma::Col<size_t> bucketContentSize;

  //! For a particular hash value, points to the row in secondHashTable
  //! corresponding to this value. Length secondHashSize.
  arma::Col<size_t> bucketRowInHashTable;

  //! The number of distance evaluations.
  size_t distanceEvaluations;
}; // class LSHSearch

} // namespace neighbor
} // namespace mlpack

//! Set the serialization version of the LSHSearch class.
BOOST_TEMPLATE_CLASS_VERSION(template<typename SortPolicy>,
    mlpack::neighbor::LSHSearch<SortPolicy>, 1);

// Include implementation.
#include "lsh_search_impl.hpp"

#endif
