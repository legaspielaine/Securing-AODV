/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Based on 
 *      NS-2 AODV model developed by the CMU/MONARCH group and optimized and
 *      tuned by Samir Das and Mahesh Marina, University of Cincinnati;
 * 
 *      AODV-UU implementation by Erik Nordström of Uppsala University
 *      http://core.it.uu.se/core/index.php/AODV-UU
 *
 * Authors: Elena Buchatskaia <borovkovaes@iitp.ru>
 *          Pavel Boyko <boyko@iitp.ru>
 */
#include "aodv-packet.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"

namespace ns3
{
	
namespace aodv
{

NS_OBJECT_ENSURE_REGISTERED (TypeHeader);

TypeHeader::TypeHeader (MessageType t) :
  m_type (t), m_valid (true)
{
}

TypeId
TypeHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::TypeHeader")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<TypeHeader> ()
  ;
  return tid;
}

TypeId
TypeHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
TypeHeader::GetSerializedSize () const
{
  return 1;
}

void
TypeHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 ((uint8_t) m_type);
}

uint32_t
TypeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t type = i.ReadU8 ();
  m_valid = true;
  switch (type)
    {
	
    case AODVTYPE_RREQ:
    case AODVTYPE_RREP:
    case AODVTYPE_RERR:
    case AODVTYPE_RREP_ACK:
      {
        m_type = (MessageType) type;
        break;
      }
	case AODVTYPE_RREQDSE: 
	  {
        m_type = (MessageType) type;
        break;
      }	
	case AODVTYPE_DSEREQ:
	  {
        m_type = (MessageType) type;
        break;
      }		
    default:
      m_valid = false;
    }
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
TypeHeader::Print (std::ostream &os) const
{
  switch (m_type)
    {
    case AODVTYPE_RREQ:
      {
        os << "RREQ";
        break;
      }
    case AODVTYPE_RREP:
      {
        os << "RREP";
        break;
      }
    case AODVTYPE_RERR:
      {
        os << "RERR";
        break;
      }
    case AODVTYPE_RREP_ACK:
      {
        os << "RREP_ACK";
        break;
      }
	case AODVTYPE_RREQDSE:
      {
        os << "RREQDSE";
        break;
      }
    default:
      os << "UNKNOWN_TYPE";
    }
}

bool
TypeHeader::operator== (TypeHeader const & o) const
{
  return (m_type == o.m_type && m_valid == o.m_valid);
}

std::ostream &
operator<< (std::ostream & os, TypeHeader const & h)
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// RREQ
//-----------------------------------------------------------------------------
RreqHeader::RreqHeader (uint8_t flags, uint8_t reserved, uint8_t hopCount, uint32_t requestID, Ipv4Address dst,
                        uint32_t dstSeqNo, Ipv4Address origin, uint32_t originSeqNo, uint64_t timeSendReq, uint64_t timeRecvReq) : //added el
  m_flags (flags), m_reserved (reserved), m_hopCount (hopCount), m_requestID (requestID), m_dst (dst),
  m_dstSeqNo (dstSeqNo), m_origin (origin),  m_originSeqNo (originSeqNo), m_timeSendReq (timeSendReq),
  m_timeRecvReq (timeRecvReq)
{
}

NS_OBJECT_ENSURE_REGISTERED (RreqHeader);

TypeId
RreqHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RreqHeader")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<RreqHeader> ()
  ;
  return tid;
}

TypeId
RreqHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RreqHeader::GetSerializedSize () const
{
  return 39; //23 without timeSend/RecvReq
}

void
RreqHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_reserved);
  i.WriteU8 (m_hopCount);
  i.WriteHtonU32 (m_requestID);
  WriteTo (i, m_dst);
  i.WriteHtonU32 (m_dstSeqNo);
  WriteTo (i, m_origin);
  i.WriteHtonU32 (m_originSeqNo);
  i.WriteHtonU64 (m_timeSendReq); //added el
  i.WriteHtonU64 (m_timeRecvReq); //added el
}

uint32_t
RreqHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_flags = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  m_requestID = i.ReadNtohU32 ();
  ReadFrom (i, m_dst);
  m_dstSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  m_originSeqNo = i.ReadNtohU32 ();
  m_timeSendReq = i.ReadNtohU64 (); //added el
  m_timeRecvReq = i.ReadNtohU64 (); //added el

  uint32_t dist = i.GetDistanceFrom (start);
 
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RreqHeader::Print (std::ostream &os) const
{
  os << "RREQ ID " << m_requestID << " destination: ipv4 " << m_dst
     << " sequence number " << m_dstSeqNo << " source: ipv4 "
     << m_origin << " sequence number " << m_originSeqNo
     << " flags:" << " Gratuitous RREP " << (*this).GetGratiousRrep ()
     << " Destination only " << (*this).GetDestinationOnly ()
     << " Unknown sequence number " << (*this).GetUnknownSeqno ();
}

std::ostream &
operator<< (std::ostream & os, RreqHeader const & h)
{
  h.Print (os);
  return os;
}

void
RreqHeader::SetGratiousRrep (bool f)
{
  if (f)
    m_flags |= (1 << 5);
  else
    m_flags &= ~(1 << 5);
}

bool
RreqHeader::GetGratiousRrep () const
{
  return (m_flags & (1 << 5));
}

void
RreqHeader::SetDestinationOnly (bool f)
{
  if (f)
    m_flags |= (1 << 4);
  else
    m_flags &= ~(1 << 4);
}

bool
RreqHeader::GetDestinationOnly () const
{
  return (m_flags & (1 << 4));
}

void
RreqHeader::SetUnknownSeqno (bool f)
{
  if (f)
    m_flags |= (1 << 3);
  else
    m_flags &= ~(1 << 3);
}

bool
RreqHeader::GetUnknownSeqno () const
{
  return (m_flags & (1 << 3));
}

bool
RreqHeader::operator== (RreqHeader const & o) const
{
  return (m_flags == o.m_flags && m_reserved == o.m_reserved &&
          m_hopCount == o.m_hopCount && m_requestID == o.m_requestID &&
          m_dst == o.m_dst && m_dstSeqNo == o.m_dstSeqNo &&
          m_origin == o.m_origin && m_originSeqNo == o.m_originSeqNo &&
          m_timeSendReq == o.m_timeSendReq && m_timeRecvReq == o.m_timeRecvReq);
}

//-----------------------------------------------------------------------------
// RREQ-DSE
//-----------------------------------------------------------------------------
RreqDSE::RreqDSE (uint32_t seed, uint32_t maxhopcount, uint32_t tophash, uint32_t hopcount, uint8_t thhashed, uint32_t oseed,
					uint64_t pubKey1, uint64_t pubKey2, uint64_t signature) :
  m_seed (seed), m_maxhopcount (maxhopcount), m_tophash (tophash), m_hopcount(hopcount), m_thhashed(thhashed), m_oseed (oseed),
  m_pubKey1 (pubKey1), m_pubKey2 (pubKey2), m_signature (signature)
{
}

NS_OBJECT_ENSURE_REGISTERED (RreqDSE);

TypeId
RreqDSE::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RreqDSE")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<RreqDSE> ()
  ;
  return tid;
}

TypeId
RreqDSE::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RreqDSE::GetSerializedSize () const
{
  return 45; //21 before edit: added el
}

void
RreqDSE::Serialize (Buffer::Iterator i) const
{

  i.WriteHtonU32 (m_seed);

  i.WriteHtonU32 (m_maxhopcount);

  i.WriteHtonU32 (m_tophash);
  
  i.WriteHtonU32 (m_hopcount);
  
  i.WriteU8 (m_thhashed);
  
  i.WriteHtonU32 (m_oseed);
  
  i.WriteHtonU64 (m_pubKey1);
  
  i.WriteHtonU64 (m_pubKey2);
  
  i.WriteHtonU64 (m_signature);
}

uint32_t
RreqDSE::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seed = i.ReadNtohU32 ();
  m_maxhopcount = i.ReadNtohU32 ();
  m_tophash = i.ReadNtohU32 ();
  m_hopcount = i.ReadNtohU32 ();
  m_thhashed = i.ReadU8 ();
  m_oseed = i.ReadNtohU32 ();
  m_pubKey1 = i.ReadNtohU64 ();
  m_pubKey2 = i.ReadNtohU64 ();
  m_signature = i.ReadNtohU64 ();
  uint32_t dist = i.GetDistanceFrom (start);
  //NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RreqDSE::Print (std::ostream &os) const
{
  os << "RREQ DSE seed " << m_seed << " Max Hop Count: " << m_maxhopcount
     << " Top Hash: " << m_tophash << "Hop Count from RREQ: " << m_hopcount;
}

std::ostream &
operator<< (std::ostream & os, RreqDSE const & h)
{
  h.Print (os);
  return os;
}

bool
RreqDSE::operator== (RreqDSE const & o) const
{
  return (m_seed == o.m_seed && m_maxhopcount == o.m_maxhopcount &&
          m_tophash == o.m_tophash && m_hopcount == o.m_hopcount && 
		  m_thhashed == o.m_thhashed && m_oseed == o.m_oseed &&
		  m_pubKey1 == o.m_pubKey1 && m_pubKey2 == o.m_pubKey2 &&
		  m_signature == o.m_signature);
}



//-----------------------------------------------------------------------------
// RREP
//-----------------------------------------------------------------------------

RrepHeader::RrepHeader (uint8_t prefixSize, uint8_t hopCount, Ipv4Address dst,
                        uint32_t dstSeqNo, Ipv4Address origin, Time lifeTime) :
  m_flags (0), m_prefixSize (prefixSize), m_hopCount (hopCount),
  m_dst (dst), m_dstSeqNo (dstSeqNo), m_origin (origin)
{
  m_lifeTime = uint32_t (lifeTime.GetMilliSeconds ());
}

NS_OBJECT_ENSURE_REGISTERED (RrepHeader);

TypeId
RrepHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RrepHeader")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<RrepHeader> ()
  ;
  return tid;
}

TypeId
RrepHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RrepHeader::GetSerializedSize () const
{
  return 19;
}

void
RrepHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_prefixSize);
  i.WriteU8 (m_hopCount);
  WriteTo (i, m_dst);
  i.WriteHtonU32 (m_dstSeqNo);
  WriteTo (i, m_origin);
  i.WriteHtonU32 (m_lifeTime);
}

uint32_t
RrepHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_flags = i.ReadU8 ();
  m_prefixSize = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  ReadFrom (i, m_dst);
  m_dstSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  m_lifeTime = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RrepHeader::Print (std::ostream &os) const
{
  os << "destination: ipv4 " << m_dst << " sequence number " << m_dstSeqNo;
  if (m_prefixSize != 0)
    {
      os << " prefix size " << m_prefixSize;
    }
  os << " source ipv4 " << m_origin << " lifetime " << m_lifeTime
     << " acknowledgment required flag " << (*this).GetAckRequired ();
}

void
RrepHeader::SetLifeTime (Time t)
{
  m_lifeTime = t.GetMilliSeconds ();
}

Time
RrepHeader::GetLifeTime () const
{
  Time t (MilliSeconds (m_lifeTime));
  return t;
}

void
RrepHeader::SetAckRequired (bool f)
{
  if (f)
    m_flags |= (1 << 6);
  else
    m_flags &= ~(1 << 6);
}

bool
RrepHeader::GetAckRequired () const
{
  return (m_flags & (1 << 6));
}

void
RrepHeader::SetPrefixSize (uint8_t sz)
{
  m_prefixSize = sz;
}

uint8_t
RrepHeader::GetPrefixSize () const
{
  return m_prefixSize;
}

bool
RrepHeader::operator== (RrepHeader const & o) const
{
  return (m_flags == o.m_flags && m_prefixSize == o.m_prefixSize &&
          m_hopCount == o.m_hopCount && m_dst == o.m_dst && m_dstSeqNo == o.m_dstSeqNo &&
          m_origin == o.m_origin && m_lifeTime == o.m_lifeTime);
}

void
RrepHeader::SetHello (Ipv4Address origin, uint32_t srcSeqNo, Time lifetime)
{
  m_flags = 0;
  m_prefixSize = 0;
  m_hopCount = 0;
  m_dst = origin;
  m_dstSeqNo = srcSeqNo;
  m_origin = origin;
  m_lifeTime = lifetime.GetMilliSeconds ();
}

std::ostream &
operator<< (std::ostream & os, RrepHeader const & h)
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// RREP-ACK
//-----------------------------------------------------------------------------

RrepAckHeader::RrepAckHeader () :
  m_reserved (0)
{
}

NS_OBJECT_ENSURE_REGISTERED (RrepAckHeader);
  
TypeId
RrepAckHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RrepAckHeader")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<RrepAckHeader> ()
  ;
  return tid;
}

TypeId
RrepAckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RrepAckHeader::GetSerializedSize () const
{
  return 1;
}

void
RrepAckHeader::Serialize (Buffer::Iterator i ) const
{
  i.WriteU8 (m_reserved);
}

uint32_t
RrepAckHeader::Deserialize (Buffer::Iterator start )
{
  Buffer::Iterator i = start;
  m_reserved = i.ReadU8 ();
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RrepAckHeader::Print (std::ostream &os ) const
{
}

bool
RrepAckHeader::operator== (RrepAckHeader const & o ) const
{
  return m_reserved == o.m_reserved;
}

std::ostream &
operator<< (std::ostream & os, RrepAckHeader const & h )
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// DSEReq
//-----------------------------------------------------------------------------

DSEReq::DSEReq (uint32_t reserved) :
  m_reserved (reserved)
{
}

NS_OBJECT_ENSURE_REGISTERED (DSEReq);
  
TypeId
DSEReq::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::DSEReq")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<DSEReq> ()
  ;
  return tid;
}

TypeId
DSEReq::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
DSEReq::GetSerializedSize () const
{
  return 4;
}

void
DSEReq::Serialize (Buffer::Iterator i ) const
{
  i.WriteHtonU32 (m_reserved);
}

uint32_t
DSEReq::Deserialize (Buffer::Iterator start )
{
  Buffer::Iterator i = start;
  m_reserved = i.ReadNtohU32 ();
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
DSEReq::Print (std::ostream &os ) const
{
	os << "Reserved " << m_reserved;
}

bool
DSEReq::operator== (DSEReq const & o ) const
{
  return m_reserved == o.m_reserved;
}

std::ostream &
operator<< (std::ostream & os, DSEReq const & h )
{
  h.Print (os);
  return os;
}
//-----------------------------------------------------------------------------
// RERR
//-----------------------------------------------------------------------------
RerrHeader::RerrHeader () :
  m_flag (0), m_reserved (0)
{
}

NS_OBJECT_ENSURE_REGISTERED (RerrHeader);

TypeId
RerrHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RerrHeader")
    .SetParent<Header> ()
    .SetGroupName("Aodv")
    .AddConstructor<RerrHeader> ()
  ;
  return tid;
}

TypeId
RerrHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RerrHeader::GetSerializedSize () const
{
  return (3 + 8 * GetDestCount ());
}

void
RerrHeader::Serialize (Buffer::Iterator i ) const
{
  i.WriteU8 (m_flag);
  i.WriteU8 (m_reserved);
  i.WriteU8 (GetDestCount ());
  std::map<Ipv4Address, uint32_t>::const_iterator j;
  for (j = m_unreachableDstSeqNo.begin (); j != m_unreachableDstSeqNo.end (); ++j)
    {
      WriteTo (i, (*j).first);
      i.WriteHtonU32 ((*j).second);
    }
}

uint32_t
RerrHeader::Deserialize (Buffer::Iterator start )
{
  Buffer::Iterator i = start;
  m_flag = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  uint8_t dest = i.ReadU8 ();
  m_unreachableDstSeqNo.clear ();
  Ipv4Address address;
  uint32_t seqNo;
  for (uint8_t k = 0; k < dest; ++k)
    {
      ReadFrom (i, address);
      seqNo = i.ReadNtohU32 ();
      m_unreachableDstSeqNo.insert (std::make_pair (address, seqNo));
    }

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RerrHeader::Print (std::ostream &os ) const
{
  os << "Unreachable destination (ipv4 address, seq. number):";
  std::map<Ipv4Address, uint32_t>::const_iterator j;
  for (j = m_unreachableDstSeqNo.begin (); j != m_unreachableDstSeqNo.end (); ++j)
    {
      os << (*j).first << ", " << (*j).second;
    }
  os << "No delete flag " << (*this).GetNoDelete ();
}

void
RerrHeader::SetNoDelete (bool f )
{
  if (f)
    m_flag |= (1 << 0);
  else
    m_flag &= ~(1 << 0);
}

bool
RerrHeader::GetNoDelete () const
{
  return (m_flag & (1 << 0));
}

bool
RerrHeader::AddUnDestination (Ipv4Address dst, uint32_t seqNo )
{
  if (m_unreachableDstSeqNo.find (dst) != m_unreachableDstSeqNo.end ())
    return true;

  NS_ASSERT (GetDestCount () < 255); // can't support more than 255 destinations in single RERR
  m_unreachableDstSeqNo.insert (std::make_pair (dst, seqNo));
  return true;
}

bool
RerrHeader::RemoveUnDestination (std::pair<Ipv4Address, uint32_t> & un )
{
  if (m_unreachableDstSeqNo.empty ())
    return false;
  std::map<Ipv4Address, uint32_t>::iterator i = m_unreachableDstSeqNo.begin ();
  un = *i;
  m_unreachableDstSeqNo.erase (i);
  return true;
}

void
RerrHeader::Clear ()
{
  m_unreachableDstSeqNo.clear ();
  m_flag = 0;
  m_reserved = 0;
}

bool
RerrHeader::operator== (RerrHeader const & o ) const
{
  if (m_flag != o.m_flag || m_reserved != o.m_reserved || GetDestCount () != o.GetDestCount ())
    return false;

  std::map<Ipv4Address, uint32_t>::const_iterator j = m_unreachableDstSeqNo.begin ();
  std::map<Ipv4Address, uint32_t>::const_iterator k = o.m_unreachableDstSeqNo.begin ();
  for (uint8_t i = 0; i < GetDestCount (); ++i)
    {
      if ((j->first != k->first) || (j->second != k->second))
        return false;

      j++;
      k++;
    }
  return true;
}

std::ostream &
operator<< (std::ostream & os, RerrHeader const & h )
{
  h.Print (os);
  return os;
}
}
}
